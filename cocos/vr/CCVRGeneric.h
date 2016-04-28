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

#include "vr/CCVRProtocol.h"
#include "renderer/CCCustomCommand.h"

NS_CC_BEGIN

class GLProgramState;

class CC_DLL VRGeneric : public VRProtocol
{
public:
    /**The max number of vertices in a vertex buffer object.*/
    static const int VBO_SIZE = 65536;
    /**The max number of indices in a index buffer.*/
    static const int INDEX_VBO_SIZE = VBO_SIZE * 6 / 4;

    VRGeneric();
    virtual ~VRGeneric();

    virtual void setup();
    virtual void cleanup();
    virtual void render(Scene* scene, Renderer* renderer);

protected:
    void onLeftDraw(Mat4 &transform, uint32_t flags);
    void onRightDraw(Mat4 &transform, uint32_t flags);
    void onAfterDraw(Mat4 &transform, uint32_t flags);

    void setupFramebuffer();
    void setupCommands();
    void setupVBO();

    GLint _textureId;
    GLint _renderbufferId;
    GLint _framebufferId;
    GLint _originalFramebufferId;
    GLint _viewport[4];
    GLboolean _cullFaceEnabled;
    GLboolean _scissorTestEnabled;

    GLProgramState* _programState;
    CustomCommand _leftEye;
    CustomCommand _rightEye;
    CustomCommand _afterDraw;

    //for TrianglesCommand
    V3F_C4B_T2F _verts[VBO_SIZE];
    GLushort _indices[INDEX_VBO_SIZE];
    GLuint _buffersVBO[2]; //0: vertex  1: indices
};

NS_CC_END
