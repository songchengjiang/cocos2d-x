/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "platform/CCPlatformMacros.h"
#include "vr/CCVRGeneric.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/ccGLStateCache.h"
#include "base/CCDirector.h"
#include "2d/CCScene.h"
#include "2d/CCCamera.h"
#include "2d/CCSprite.h"
#include "platform/CCGLView.h"


NS_CC_BEGIN

VRGeneric::VRGeneric()
{
}

VRGeneric::~VRGeneric()
{
    CC_SAFE_RELEASE(_leftSprite);
    CC_SAFE_RELEASE(_rightSprite);
}

void VRGeneric::setup(GLView* glview)
{
//    CC_UNUSED(glview);

    // set origin to 0,0 in case origin is not 0,0
    auto vp = Camera::getDefaultViewport();
    _vrViewSize.width = vp._width/2 + vp._left;
    _vrViewSize.height = vp._height/2 + vp._bottom;
    vp._left = vp._bottom = 0;
    Camera::setDefaultViewport(vp);

    _texSize = Size(vp._width, vp._height);

    _leftFB = experimental::FrameBuffer::create(1, _texSize.width, _texSize.height);
    _leftFB->retain();
    auto leftRT = experimental::RenderTarget::create(_texSize.width, _texSize.height);
    auto leftDS = experimental::RenderTargetDepthStencil::create(_texSize.width, _texSize.height);
    _leftFB->attachRenderTarget(leftRT);
    _leftFB->attachDepthStencilTarget(leftDS);
    _leftFB->setClearColor(Color4F(1,0,0,1));

    _rightFB = experimental::FrameBuffer::create(2, _texSize.width, _texSize.height);
    _rightFB->retain();
    auto rightRT = experimental::RenderTarget::create(_texSize.width, _texSize.height);
    auto rightDS = experimental::RenderTargetDepthStencil::create(_texSize.width, _texSize.height);
    _rightFB->attachRenderTarget(rightRT);
    _rightFB->attachDepthStencilTarget(rightDS);
    _rightFB->setClearColor(Color4F(0,0,1,1));

    Size scaledTex = Size(vp._width/2, vp._height/2);
    V3F_C4B_T2F_Quad quad;
    quad.bl.colors = Color4B::WHITE;
    quad.bl.texCoords = Tex2F(0,0);
    quad.bl.vertices = Vec3(0,0,0);

    quad.tl.colors = Color4B::WHITE;
    quad.tl.texCoords = Tex2F(0,1);
    quad.tl.vertices = Vec3(0,scaledTex.height,0);

    quad.br.colors = Color4B::WHITE;
    quad.br.texCoords = Tex2F(1,0);
    quad.br.vertices = Vec3(scaledTex.width,0,0);

    quad.tr.colors = Color4B::WHITE;
    quad.tr.texCoords = Tex2F(1,1);
    quad.tr.vertices = Vec3(scaledTex.width,scaledTex.height,0);

    PolygonInfo polyInfo;
    polyInfo.setQuad(&quad);

    _leftSprite = Sprite::createWithTexture(_leftFB->getRenderTarget()->getTexture());
    _leftSprite->retain();
    _leftSprite->setPosition(Vec2(0,0));
    _leftSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    _leftSprite->setPolygonInfo(polyInfo);

    _rightSprite = Sprite::createWithTexture(_rightFB->getRenderTarget()->getTexture());
    _rightSprite->retain();
    _rightSprite->setPosition(Vec2(0,0));
    _rightSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    _rightSprite->setPolygonInfo(polyInfo);
}

void VRGeneric::cleanup()
{
}

void VRGeneric::render(Scene* scene, Renderer* renderer)
{
    auto director = Director::getInstance();

    // FIXME: Use correct eye displacement
    const float eyeOffset = 1;
    _leftFB->applyFBO();
    scene->render(renderer, Vec3(-eyeOffset,0,0));
    _leftFB->restoreFBO();

    _rightFB->applyFBO();
    scene->render(renderer, Vec3(eyeOffset,0,0));
    _rightFB->restoreFBO();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    Mat4 proj2D;
    Mat4::createOrthographic(_texSize.width, _texSize.height, -1, 1, &proj2D);
    proj2D.translate(-_texSize.width/2, -_texSize.height/2, 0);
    proj2D.scale(2);
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, proj2D);


    glViewport(0, _vrViewSize.height/2 , _vrViewSize.width, _vrViewSize.height);
    _leftSprite->visit(renderer, Mat4::IDENTITY, 0);
    renderer->render();

    glViewport(_vrViewSize.width, _vrViewSize.height/2 , _vrViewSize.width, _vrViewSize.height);
    _rightSprite->visit(renderer, Mat4::IDENTITY, 0);
    renderer->render();

    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

NS_CC_END