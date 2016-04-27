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
#include "renderer/CCQuadCommand.h"

NS_CC_BEGIN

class GLProgramState;

class CC_DLL VRGeneric : public VRProtocol
{
public:
    VRGeneric();
    virtual ~VRGeneric();

    virtual void setup();
    virtual void cleanup();
    virtual void beforeDraw(Scene* scene, Renderer* renderer);
    virtual void afterDraw(Scene* scene, Renderer* renderer);

protected:
    void setupFramebuffer();
    void setupCommands();
    void setupQuad(V3F_C4B_T2F_Quad* quad);

    GLint _textureId;
    GLint _renderbufferId;
    GLint _framebufferId;
    GLint _originalFramebufferId;
    GLint _viewport[4];
    GLboolean _cullFaceEnabled;
    GLboolean _scissorTestEnabled;

    GLProgramState* _programState;
    QuadCommand _leftEye;
    QuadCommand _rightEye;
    V3F_C4B_T2F_Quad _quadLeft;
    V3F_C4B_T2F_Quad _quadRight;
};

NS_CC_END
