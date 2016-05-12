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
#include "vr/CCVRCardboard.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/ccGLStateCache.h"
#include "base/CCDirector.h"
#include "2d/CCScene.h"
#include "2d/CCCamera.h"
#include "2d/CCSprite.h"
#include "platform/CCGLView.h"
#include "platform/android/jni/JniHelper.h"


NS_CC_BEGIN

VRCardboard::VRCardboard()
{
}

VRCardboard::~VRCardboard()
{
}

void VRCardboard::setup(GLView* glview)
{
    CCLOG("JNIENV: 0x%x, ACTIVITY: 0x%x, JAVAVM: 0x%x", (int)JniHelper::getEnv(), (int)JniHelper::getActivity(), (int)JniHelper::getJavaVM());
    cbJava java;
    java.ActivityObject = JniHelper::getEnv()->NewGlobalRef(JniHelper::getActivity());
    java.Env = JniHelper::getEnv();
    JniHelper::getEnv()->GetJavaVM(&java.Vm);
    if (cbapi_Initialize(&java, &_hmd)){
        _eyes = cbapi_GetEyes();
    }
    cbapi_EnterVrMode();
}

void VRCardboard::cleanup()
{
    cbapi_LeaveVrMode();
}

void VRCardboard::render(Scene* scene, Renderer* renderer)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    cbapi_beforeDrawFrame();
    glEnable(GL_SCISSOR_TEST);
    glDepthMask(true);
    for (unsigned short i = 0; i < CB_EYE_NUM; ++i){
        auto vp = _eyes.eyeParams[i].viewport;
        Camera::setDefaultViewport(experimental::Viewport(vp.x, vp.y, vp.width, vp.height));
        glScissor(vp.x, vp.y, vp.width, vp.height);
        glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const float eyeOffset = ( i ? -0.5f : 0.5f ) * _hmd.device.interLensDistance;
        scene->render(renderer, Vec3(eyeOffset,0,0));
    }
    glDepthMask(false);
    glDisable(GL_SCISSOR_TEST);
    cbapi_afterDrawFrame();
    
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

NS_CC_END