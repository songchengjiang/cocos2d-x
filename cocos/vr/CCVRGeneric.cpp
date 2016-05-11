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
#include "vr/CCVRDistortionMesh.h"
#include "vr/CCVRDistortion.h"
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
    CC_SAFE_RELEASE(_fbSprite);
    CC_SAFE_RELEASE(_fb);
}

void VRGeneric::setup(GLView* glview)
{
//    CC_UNUSED(glview);

    // set origin to 0,0 in case origin is not 0,0
    auto vp = Camera::getDefaultViewport();

    _leftEye.viewport._bottom = _leftEye.viewport._left = 0;
    _leftEye.viewport._width = vp._width/2;
    _leftEye.viewport._height = vp._height/2;

    _rightEye.viewport._bottom = 0;
    _rightEye.viewport._left = _leftEye.viewport._width;
    _rightEye.viewport._width = vp._width/2;
    _rightEye.viewport._height = vp._height/2;


    _texSize = Size(vp._width, vp._height);

    _fb = experimental::FrameBuffer::create(1, _texSize.width, _texSize.height);
    _fb->retain();
    auto rt = experimental::RenderTarget::create(_texSize.width, _texSize.height);
    auto ds = experimental::RenderTargetDepthStencil::create(_texSize.width, _texSize.height);
    _fb->attachRenderTarget(rt);
    _fb->attachDepthStencilTarget(ds);
    _fb->setClearColor(Color4F(1,0,0,1));

    auto director = Director::getInstance();
    Size scaledTex = director->getWinSize();
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

    _fbSprite = Sprite::createWithTexture(_fb->getRenderTarget()->getTexture());
    _fbSprite->retain();
    _fbSprite->setPosition(Vec2(0,0));
    _fbSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    _fbSprite->setPolygonInfo(polyInfo);
}

void VRGeneric::cleanup()
{
}

void VRGeneric::render(Scene* scene, Renderer* renderer)
{
    auto origVP = Camera::getDefaultViewport();

    // FIXME: Use correct eye displacement
    const float eyeOffset = 1;
    _fb->applyFBO();
    Camera::setDefaultViewport(_leftEye.viewport);
    scene->render(renderer, Vec3(-eyeOffset,0,0));
    Camera::setDefaultViewport(_rightEye.viewport);
    scene->render(renderer, Vec3(eyeOffset,0,0));
    _fb->restoreFBO();

    Camera::setDefaultViewport(origVP);
    Camera::getDefaultCamera()->apply();
    _fbSprite->visit(renderer, Mat4::IDENTITY, 0);
    renderer->render();
    Camera::getDefaultCamera()->restore();
}

DistortionMesh* VRGeneric::createDistortionMesh(const experimental::Viewport& eyeViewport,
                                                float textureWidthTanAngle,
                                                float textureHeightTanAngle,
                                                float xEyeOffsetTanAngleScreen,
                                                float yEyeOffsetTanAngleScreen)
{
    return nullptr;
//    return new DistortionMesh(_distortion,
//                              _distortion,
//                              _distortion,
//                              _headMountedDisplay->getScreen()->widthInMeters() / _metersPerTanAngle,
//                              _headMountedDisplay->getScreen()->heightInMeters() / _metersPerTanAngle,
//                              xEyeOffsetTanAngleScreen, yEyeOffsetTanAngleScreen,
//                              textureWidthTanAngle, textureHeightTanAngle,
//                              eyeViewport->eyeX, eyeViewport->eyeY,
//                              eyeViewport._left, eyeViewport._bottom,
//                              eyeViewport._width, eyeViewport._height,
//                              _vignetteEnabled);
}
NS_CC_END