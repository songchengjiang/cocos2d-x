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
: _vignetteEnabled(true)
, _distortion(nullptr)
, _leftDistortionMesh(nullptr)
, _rightDistortionMesh(nullptr)
, _glProgramState(nullptr)
, _resolutionScale(1)
{
}

VRGeneric::~VRGeneric()
{
    CC_SAFE_RELEASE(_glProgramState);
    CC_SAFE_RELEASE(_fb);
    CC_SAFE_DELETE(_distortion);
    CC_SAFE_DELETE(_leftDistortionMesh);
    CC_SAFE_DELETE(_rightDistortionMesh);
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


    _distortion = new Distortion;
    float xEyeOffsetTanAngleScreen = 0.33;
    float yEyeOffsetTanAngleScreen = 0.69;
    float textureWidht = 3.35;
    float textureHeight = 1.67;
    _leftDistortionMesh = createDistortionMesh(_leftEye.viewport,
                                               textureWidht,
                                               textureHeight,
                                               xEyeOffsetTanAngleScreen,
                                               yEyeOffsetTanAngleScreen);

    xEyeOffsetTanAngleScreen = 1.76;
    _rightDistortionMesh = createDistortionMesh(_rightEye.viewport,
                                                textureWidht,
                                                textureHeight,
                                                xEyeOffsetTanAngleScreen,
                                                yEyeOffsetTanAngleScreen);

    setupGLProgram();
}

void VRGeneric::cleanup()
{
}

void VRGeneric::render(Scene* scene, Renderer* renderer)
{
    // FIXME: Use correct eye displacement
    const float eyeOffset = 1;
    _fb->applyFBO();
    Camera::setDefaultViewport(_leftEye.viewport);
    scene->render(renderer, Vec3(-eyeOffset,0,0));
    Camera::setDefaultViewport(_rightEye.viewport);
    scene->render(renderer, Vec3(eyeOffset,0,0));
    _fb->restoreFBO();

    auto texture = _fb->getRenderTarget()->getTexture();
    GL::bindTexture2D(texture->getName());
    _glProgramState->apply(Mat4::IDENTITY);

    GLint origViewport[4];
    glGetIntegerv(GL_VIEWPORT, origViewport);
    glViewport(0, 0, _texSize.width, _texSize.height);

    renderDistortionMesh(_leftDistortionMesh, texture->getName());
    renderDistortionMesh(_rightDistortionMesh, texture->getName());


    glViewport(origViewport[0], origViewport[1], origViewport[2], origViewport[3]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERROR_DEBUG();
}

void VRGeneric::renderDistortionMesh(DistortionMesh *mesh, GLint textureID)
{
    glBindBuffer(GL_ARRAY_BUFFER, mesh->_arrayBufferID);

    _glProgramState->setVertexAttribPointer("a_position", 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(0 * sizeof(float)));
    _glProgramState->setVertexAttribPointer("a_vignette", 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(2 * sizeof(float)));
    _glProgramState->setVertexAttribPointer("a_blueTextureCoord", 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(7 * sizeof(float)));
    _glProgramState->setUniformTexture("u_textureSampler", textureID);
    _glProgramState->setUniformFloat("u_textureCoordScale", _resolutionScale);

    _glProgramState->apply(Mat4::IDENTITY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->_elementBufferID);
    glDrawElements(GL_TRIANGLE_STRIP, mesh->_indices, GL_UNSIGNED_SHORT, 0);

    CHECK_GL_ERROR_DEBUG();
}

DistortionMesh* VRGeneric::createDistortionMesh(const experimental::Viewport& eyeViewport,
                                                float textureWidthTanAngle,
                                                float textureHeightTanAngle,
                                                float xEyeOffsetTanAngleScreen,
                                                float yEyeOffsetTanAngleScreen)
{
    const float displayWidth = 2.1;
    const float displayHeight = 1.18;
    const float eyeViewportLeft = 0.839;
    const float eyeViewportRight = 0.839;

    return new DistortionMesh(_distortion,
                              _distortion,
                              _distortion,
                              displayWidth,
                              displayHeight,
//                              _headMountedDisplay->getScreen()->widthInMeters() / _metersPerTanAngle,
//                              _headMountedDisplay->getScreen()->heightInMeters() / _metersPerTanAngle,
                              xEyeOffsetTanAngleScreen, yEyeOffsetTanAngleScreen,
                              textureWidthTanAngle, textureHeightTanAngle,
                              eyeViewportLeft, eyeViewportRight,
//                              eyeViewport->eyeX, eyeViewport->eyeY,
                              0, 0,
                              1.67, 1.67,
                              _vignetteEnabled);
}

void VRGeneric::setupGLProgram()
{
    const GLchar *vertexShader =
    "\
    attribute vec2 a_position;\n\
    attribute float a_vignette;\n\
    attribute vec2 a_blueTextureCoord;\n\
    varying vec2 v_textureCoord;\n\
    varying float v_vignette;\n\
    uniform float u_textureCoordScale;\n\
    void main() {\n\
    gl_Position = vec4(a_position, 0.0, 1.0);\n\
    v_textureCoord = a_blueTextureCoord.xy * u_textureCoordScale;\n\
    v_vignette = a_vignette;\n\
    }\n";

    const GLchar *fragmentShader =
    "\
    #ifdef GL_ES\n\
    precision mediump float;\n\
    #endif\n\
    varying vec2 v_textureCoord;\n\
    varying float v_vignette;\n\
    uniform sampler2D u_textureSampler;\n\
    void main() {\n\
    gl_FragColor = v_vignette * texture2D(u_textureSampler, v_textureCoord);\n\
    }\n";

    auto program = GLProgram::createWithByteArrays(vertexShader, fragmentShader);
    _glProgramState = GLProgramState::getOrCreateWithGLProgram(program);

    _glProgramState->retain();
}
NS_CC_END
