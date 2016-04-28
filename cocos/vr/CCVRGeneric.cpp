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
    setupVBO();
}

void Renderer::setupVBO()
{
    // init indices
    for (int i=0; i < INDEX_VBO_SIZE/6; i++)
    {
        _indices[i*6+0] = (GLushort) (i*4+0);
        _indices[i*6+1] = (GLushort) (i*4+1);
        _indices[i*6+2] = (GLushort) (i*4+2);
        _indices[i*6+3] = (GLushort) (i*4+3);
        _indices[i*6+4] = (GLushort) (i*4+2);
        _indices[i*6+5] = (GLushort) (i*4+1);
    }

    // init vertices
    for (int i=0; i < VBO_SIZE/4; i++)
    {
        // bottom left
        _verts[i*4+0].colors = Color4B::WHITE;
        _verts[i*4+0].texCoords = {0,0};
        _verts[i*4+0].vertices = {0,0,0};

        // bottom right
        _verts[i*4+1].colors = Color4B::WHITE;
        _verts[i*4+1].texCoords = {1,0};
        _verts[i*4+1].vertices = {0,0,0};

        // bottom right
        _verts[i*4+0].colors = Color4B::WHITE;
        _verts[i*4+0].texCoords = {1,0};
        _verts[i*4+0].vertices = {0,0,0};
    }

    // create buffers
    glGenBuffers(2, &_buffersVBO[0]);

    // Avoid changing the element buffer for whatever VAO might be bound.
    GL::bindVAO(0);

    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_verts[0]) * VBO_SIZE, _verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * INDEX_VBO_SIZE, _indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERROR_DEBUG();
}

void VRGeneric::setupCommands()
{
    _programState = GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP);
    _programState->retain();

    auto director = Director::getInstance();
    auto mv = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    _leftEye.init(-1000);
    _leftEye.func = CC_CALLBACK_0(VRGeneric::onLeftDraw, this, mv, 0);

//    _rightEye.init(0, mv, 0);
//    _rightEye.func = CC_CALLBACK_0(VRGeneric::onRightDraw, this, mv, 0);

    _afterDraw.init(1000);
    _afterDraw.func = CC_CALLBACK_0(VRGeneric::onAfterDraw, this, mv, 0);
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

void VRGeneric::onLeftDraw(Mat4 &transform, uint32_t flags)
{
    // bind framebuffer
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_originalFramebufferId);
    CHECK_GL_ERROR_DEBUG();

    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);
    CHECK_GL_ERROR_DEBUG();

    // save viewport
    glGetIntegerv(GL_VIEWPORT, _viewport);
    CHECK_GL_ERROR_DEBUG();

    _cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
    _scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);

    glEnable(GL_SCISSOR_TEST);
    glDepthMask(true);

    const float viewport_width = 1024;
    const float viewport_height = 1024;

    glViewport(0, 0, viewport_width / 2, viewport_height);
    glScissor(0, 0, viewport_width / 2, viewport_height);
    CHECK_GL_ERROR_DEBUG();
}

void VRGeneric::onRightDraw(Mat4 &transform, uint32_t flags)
{
    auto size = Director::getInstance()->getVisibleSize();

    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    CHECK_GL_ERROR_DEBUG();

    glClearColor(0.0F, 0.0F, 1.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
    CHECK_GL_ERROR_DEBUG();

    glEnable(GL_SCISSOR_TEST);

    glScissor(0, 0, size.width / 2, size.height);
    CHECK_GL_ERROR_DEBUG();

    // draw
    GL::bindTexture2D((GLuint)_textureId);
    GL::blendFunc(CC_BLEND_SRC, CC_BLEND_DST);
    _programState->apply(transform);

    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_verts[0]) * _filledVertex , _verts, GL_STATIC_DRAW);

    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

    // vertices
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof(V3F_C4B_T2F, vertices));
    // colors
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof(V3F_C4B_T2F, colors));
    // tex coords
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof(V3F_C4B_T2F, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * _filledIndex, _indices, GL_STATIC_DRAW);

    if (_cullFaceEnabled)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    if (_scissorTestEnabled)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
}

void VRGeneric::render(Scene* scene, Renderer* renderer)
{
    renderer->addCommand(&_leftEye);
    scene->render(renderer);
    renderer->addCommand(&_afterDraw);
}

NS_CC_END