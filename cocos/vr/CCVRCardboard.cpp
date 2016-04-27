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

#include "vr/CCVRCardboard.h"

NS_CC_BEGIN

VRCardboard::VRCardboard()
{

}

VRCardboard::~VRCardboard()
{

}

void VRCardboard::setup()
{
    cbJava java;
    /*Initialize cbJava from JNI
    ...
    */
    cbapi_Initialize(&java, &_hmd);
    
    _eyes = cbapi_GetEyes();
}

void VRCardboard::cleanup()
{

}

void VRCardboard::startFrame()
{
    cbapi_beforeDrawFrame();
    glEnable(GL_SCISSOR_TEST);
    glDepthMask(true);
    
    for (int eye = 0; eye < CB_EYE_NUM; ++eye){
        auto vp = _eyes.eyeParams[eye].viewport;
        glScissor(vp.x, vp.y, vp.width, vp.height);
        glViewport(vp.x, vp.y, vp.width, vp.height);
        glClearColor(0.125f, 0.0f, 0.125f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void VRCardboard::endFrame()
{
    for (int eye = 0; eye < CB_EYE_NUM; ++eye){
        auto vp = _eyes.eyeParams[eye].viewport;
        // Explicitly clear the border texels to black because OpenGL-ES does not support GL_CLAMP_TO_BORDER.
        {
            // Clear to fully opaque black.
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            // bottom
            glScissor(vp.x, vp.y, vp.width, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            // top
            glScissor(vp.x, vp.height - 1, vp.width, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            // left
            glScissor(vp.x, vp.y, 1, vp.height);
            glClear(GL_COLOR_BUFFER_BIT);
            // right
            glScissor(vp.x + vp.width - 1, vp.y, 1, vp.height);
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }
    
    glDepthMask(false);
    glDisable(GL_SCISSOR_TEST);
    cbapi_afterDrawFrame();
}

void VRCardboard::beforeDraw(int eye)
{
    auto vp = _eyes.eyeParams[eye].viewport;
    glScissor(vp.x, vp.y, vp.width, vp.height);
    glViewport(vp.x, vp.y, vp.width, vp.height);
}

void VRCardboard::afterDraw(int eye)
{
}

void VRCardboard::getEyeOffsetMat(int eye, Mat4 &offset)
{
    const float eyeOffset = ( eye ? -0.5f : 0.5f ) * _hmd.device.interLensDistance;
    const cbMatrix4 eyeOffsetMatrix = cbMatrix4_CreateTranslation( eyeOffset, 0.0f, 0.0f);
    offset.set((const GLfloat *)(cbMatrix4_Transpose(&eyeViewMatrix).M[0]));
}

void VRCardboard::startTracking()
{
    cbapi_EnterVrMode();
}

bool VRCardboard::getPose(Vec3 &pos, Quaternion &rot)
{
    Mat4 headView;
    headView.set((const GLfloat *)(cbMatrix4_Transpose(&cbapi_getLastHeadView()).M[0]));
    headView.decompose(nullptr, &rot, &pos);
    return true;
}

void VRCardboard::endTracking()
{
    cbapi_LeaveVrMode();
}

NS_CC_END