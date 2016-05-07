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

    // rebase default viewport in case origin is not 0,0
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

    _leftSprite = Sprite::createWithTexture(_leftFB->getRenderTarget()->getTexture());
    _leftSprite->retain();
    _leftSprite->setPosition(Vec2(0,0));
    _leftSprite->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    _leftSprite->setScaleY(-1/CC_CONTENT_SCALE_FACTOR());
    _leftSprite->setScaleX(1/CC_CONTENT_SCALE_FACTOR());
//    _leftSprite->setPolygonInfo(polyinfo);

    _rightSprite = Sprite::createWithTexture(_rightFB->getRenderTarget()->getTexture());
    _rightSprite->retain();
    _rightSprite->setPosition(Vec2(0,0));
    _rightSprite->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    _rightSprite->setScaleY(-1/CC_CONTENT_SCALE_FACTOR());
    _rightSprite->setScaleX(1/CC_CONTENT_SCALE_FACTOR());
//    _rightSprite->setPolygonInfo(polyinfo);
}

void VRGeneric::cleanup()
{
}

void VRGeneric::render(Scene* scene, Renderer* renderer)
{
    _leftFB->applyFBO();
    scene->render(renderer, Vec3(-5,0,0));
    _leftFB->restoreFBO();

    _rightFB->applyFBO();
    scene->render(renderer, Vec3(5,0,0));
    _rightFB->restoreFBO();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glViewport(0, _vrViewSize.height/2 , _vrViewSize.width, _vrViewSize.height);
    _leftSprite->visit(renderer, Mat4::IDENTITY, 0);
    renderer->render();

    glViewport(_vrViewSize.width, _vrViewSize.height/2 , _vrViewSize.width, _vrViewSize.height);
    _rightSprite->visit(renderer, Mat4::IDENTITY, 0);
    renderer->render();

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

NS_CC_END