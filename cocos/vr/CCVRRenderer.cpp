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

#include "vr/CCVRRenderer.h"
#include "base/CCDirector.h"
#include "2d/CCCamera.h"

NS_CC_BEGIN

VRRenderer::VRRenderer(VRProtocol *vrProtocol)
    : _vrProtocol(vrProtocol)
{
    _vrProtocol->retain();
}

VRRenderer::~VRRenderer()
{
    _vrProtocol->release();
}

void VRRenderer::render()
{
    //Uncomment this once everything is rendered by new renderer
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //TODO: setup camera or MVP
    _isRendering = true;
    
    if (_glViewAssigned)
    {
        //Process render commands
        //1. Sort render commands based on ID
        for (auto &renderqueue : _renderGroups)
        {
            renderqueue.sort();
        }
        
        for (int eye = 0; eye < EYE_COUNT; ++eye){
            if (Camera::getVisitingCamera()){
                Mat4 eyeOffset, viewProj;
                _vrProtocol->getEyeOffsetMat(eye, eyeOffset);
                Mat4::multiply(Camera::getVisitingCamera()->getViewProjectionMatrix(), eyeOffset, &viewProj);
                Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
                Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, viewProj);
            }
            _vrProtocol->beforeDraw(eye);
            visitRenderQueue(_renderGroups[0]);
            _vrProtocol->afterDraw(eye);
            
            if (Camera::getVisitingCamera())
                Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
        }
    }
    clean();
    
    _isRendering = false;
}

void VRRenderer::startRender()
{
    _vrProtocol->startFrame();
}

void VRRenderer::endRender()
{
    _vrProtocol->endFrame();
}

NS_CC_END