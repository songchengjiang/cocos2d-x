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

#include "vr/CCVRGenericHeadTracker.h"
#include "platform/CCPlatformMacros.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#import <CoreMotion/CoreMotion.h>
#endif

NS_CC_BEGIN

//////

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
static Mat4 matrixFromRotationMatrix(const CMRotationMatrix& rotationMatrix)
{
    return Mat4(rotationMatrix.m11,
                rotationMatrix.m12,
                rotationMatrix.m13,
                0.0f,

                rotationMatrix.m21,
                rotationMatrix.m22,
                rotationMatrix.m23,
                0.0f,

                rotationMatrix.m31,
                rotationMatrix.m32,
                rotationMatrix.m33,
                0.0f,

                0.0f,
                0.0f,
                0.0f,
                1.0f);
}
#endif

VRGenericHeadTracker::VRGenericHeadTracker()
: _localPosition(Vec3::ZERO)
, _localRotation(Mat4::IDENTITY)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    _motionMgr = [[CMMotionManager alloc] init];
    startTracking();
#endif
}

VRGenericHeadTracker::~VRGenericHeadTracker()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    stopTracking();
    [(CMMotionManager*)_motionMgr release];
#endif
}

void VRGenericHeadTracker::startTracking()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CMMotionManager* motionMgr = (CMMotionManager*)_motionMgr;
    if (motionMgr.isDeviceMotionAvailable && !motionMgr.isDeviceMotionActive)
    {
        [motionMgr startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXArbitraryZVertical];
    }

    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    if (orientation == UIInterfaceOrientationLandscapeLeft)
    {
        Quaternion quat = Quaternion(0, 0, 90, 1);
        Mat4::createRotation(quat, &_displayFromDevice);
    }
    else if (orientation == UIInterfaceOrientationLandscapeRight)
    {
        Quaternion quat = Quaternion(0, 0, -90, 1);
        Mat4::createRotation(quat, &_displayFromDevice);
    }
#endif
}

void VRGenericHeadTracker::stopTracking()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    [(CMMotionManager*)_motionMgr stopDeviceMotionUpdates];
#endif
}

Vec3 VRGenericHeadTracker::getLocalPosition()
{
    return _localPosition;
}

Mat4 VRGenericHeadTracker::getLocalRotation()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CMMotionManager* motionMgr = (CMMotionManager*)_motionMgr;
    CMDeviceMotion *motion = motionMgr.deviceMotion;
    if (!motion) { return _localRotation; }

    CMRotationMatrix rotationMatrix = motion.attitude.rotationMatrix;
    Mat4 deviceFromInertialReferenceFrame = matrixFromRotationMatrix(rotationMatrix).getTransposed(); // note the matrix inversion

    Mat4 deviceFromWorld = deviceFromInertialReferenceFrame;
    Mat4 displayFromWorld = _displayFromDevice * deviceFromWorld;

    _localRotation = displayFromWorld;

    // bug
    _localRotation = Mat4::IDENTITY;
#endif
    
    return _localRotation;
}

NS_CC_END
