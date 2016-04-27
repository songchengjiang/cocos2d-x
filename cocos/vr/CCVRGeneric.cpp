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
#include "2d/CCScene.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCGLProgramState.h"
#include "base/CCDirector.h"

NS_CC_BEGIN

VRGeneric::VRGeneric()
: _textureId(-1)
, _renderbufferId(-1)
, _framebufferId(-1)
, _originalFramebufferId(-1)
, _programState(nullptr)
{
}

VRGeneric::~VRGeneric()
{
    CC_SAFE_RELEASE(_programState);
}

void VRGeneric::setup()
{
    setupFramebuffer();
    setupCommands();
}

void VRGeneric::setupCommands()
{
    _programState = GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP);
    _programState->retain();

    auto director = Director::getInstance();
    auto mv = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    setupQuad(&_quadLeft);
    setupQuad(&_quadRight);

    _leftEye.init(0, _textureId,
                  _programState,
                   BlendFunc::ALPHA_PREMULTIPLIED,
                  &_quadLeft,
                  1,
                  mv,
                  0);

    _rightEye.init(0, _textureId,
                  _programState,
                  BlendFunc::ALPHA_PREMULTIPLIED,
                  &_quadRight,
                  1,
                  mv,
                  0);

}

void VRGeneric::setupQuad(V3F_C4B_T2F_Quad* quad)
{
    auto director = Director::getInstance();
    auto size = director->getVisibleSize();

    // clean the Quad
    memset(quad, 0, sizeof(*quad));
    // Atlas: Color
    quad->bl.colors = Color4B::WHITE;
    quad->br.colors = Color4B::WHITE;
    quad->tl.colors = Color4B::WHITE;
    quad->tr.colors = Color4B::WHITE;

    // left eye
    float x1 = 0;
    float y1 = 0;
    float x2 = size.width / 2;
    float y2 = size.height;

    if (quad == &_quadRight) {
        x1 = size.width / 2;
    }

    quad->bl.vertices = Vec3(x1, y1, 0);
    quad->br.vertices = Vec3(x2, y1, 0);
    quad->tl.vertices = Vec3(x1, y2, 0);
    quad->tr.vertices = Vec3(x2, y2, 0);

    const float left = 0;
    const float right = 1;
    const float top = 1;
    const float bottom = 0;
    
    quad->bl.texCoords.u = left;
    quad->bl.texCoords.v = top;
    quad->br.texCoords.u = left;
    quad->br.texCoords.v = bottom;
    quad->tl.texCoords.u = right;
    quad->tl.texCoords.v = top;
    quad->tr.texCoords.u = right;
    quad->tr.texCoords.v = bottom;
}

void VRGeneric::setupFramebuffer()
{
    // FIXME: this should be the Headset size, and
    // not the director size
    auto director = Director::getInstance();
    auto size = director->getVisibleSize();

    const float width = size.width / 2;
    const float height = size.height;

    // create texture
    glGenTextures(1, (GLuint*)&_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);

    CHECK_GL_ERROR_DEBUG();

    glGenRenderbuffers(1, (GLuint*)&_renderbufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderbufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

    CHECK_GL_ERROR_DEBUG();

    glGenFramebuffers(1, (GLuint*)&_framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

    CHECK_GL_ERROR_DEBUG();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureId, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderbufferId);

    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    CCASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Invalid framebuffer");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    CHECK_GL_ERROR_DEBUG();
}

void VRGeneric::cleanup()
{

}

void VRGeneric::beforeDraw(Scene* scene, Renderer* renderer)
{
    // bind framebuffer
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_originalFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

    // save viewport
    glGetIntegerv(GL_VIEWPORT, _viewport);
    _cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
    _scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);

    glEnable(GL_SCISSOR_TEST);
    glDepthMask(true);

    const float viewport_width = 1024;
    const float viewport_height = 1024;

    glViewport(0, 0, viewport_width / 2, viewport_height);
    glScissor(0, 0, viewport_width / 2, viewport_height);

    // pass left eye transform
    scene->render(renderer);

    glViewport(viewport_width / 2, 0, viewport_width / 2, viewport_width);
    glScissor(viewport_width / 2, 0, viewport_width / 2, viewport_width);

    // pass right eye transform
    scene->render(renderer);

    glBindFramebuffer(GL_FRAMEBUFFER, _originalFramebufferId);
}


void VRGeneric::afterDraw(Scene* scene, Renderer* renderer)
{
    // flush
    renderer->render();

    glBindFramebuffer(GL_FRAMEBUFFER, _originalFramebufferId);
    glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);

    renderer->addCommand(&_leftEye);
    renderer->addCommand(&_rightEye);
}

NS_CC_END