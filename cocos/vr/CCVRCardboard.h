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

#ifndef _CCVRCARDBOARD_H__
#define _CCVRCARDBOARD_H__
#include "vr/CCVRProtocol.h"
#include "vr/cardboard/CbApi.h"

NS_CC_BEGIN

class CC_DLL VRCardboard : public VRProtocol
{
public:
    
    VRCardboard();
    virtual ~VRCardboard();
    
    virtual void setup() override;
    virtual void cleanup() override;
    virtual void startFrame() override;
    virtual void endFrame() override;
    virtual void beforeDraw(int eye) override;
    virtual void afterDraw(int eye) override;
    virtual void getEyeOffsetMat(int eye, Mat4 &offset) override;
    
    virtual void startTracking() override;
    virtual bool getPose(Vec3 &pos, Quaternion &rot) override;
    virtual void endTracking() override;
    
private:
    
    cbHeadMountedDisplay _hmd;
    cbEyes               _eyes;
};

NS_CC_END

#endif
