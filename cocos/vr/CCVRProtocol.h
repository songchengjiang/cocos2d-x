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

#ifndef _CCVRPROTOCOL_H__
#define _CCVRPROTOCOL_H__
#include "base/ccTypes.h"
#include "math/Vec3.h"
#include "math/Quaternion.h"
#include "math/Mat4.h"

NS_CC_BEGIN

class CC_DLL VRProtocol : public Ref
{
public:
    virtual ~VRProtocol(){}
    
    virtual void setup() = 0;
    virtual void cleanup() = 0;
    virtual void startFrame() = 0;
    virtual void endFrame() = 0;
    virtual void beforeDraw(int eye) = 0;
    virtual void afterDraw(int eye) = 0;
    virtual void getEyeOffsetMat(int eye, Mat4 &offset) = 0;
    
    virtual void startTracking() = 0;
    virtual bool getPose(Vec3 &pos, Quaternion &rot) = 0;
    virtual void endTracking() = 0;
};

NS_CC_END


#endif
