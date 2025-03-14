////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2025 Mateusz Malinowski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <generic_talker.h>
#include "abstract_robot_base.h"


ARobotBase::ARobotBase(const char* name, const float steeringGain, const float steeringOffset, const float throttleGain)
: mName(name),
  mSteering(0.0f),
  mThrottle(0.0f),
  mSteeringGain(steeringGain),
  mSteeringOffset(steeringOffset),
  mThrottleGain(throttleGain),
  mI2C(),
  mThrottlePCA(&mI2C, PCA9685_ADDRESS_2)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mMutex, &attr);
}

ARobotBase::~ARobotBase()
{
    pthread_mutex_destroy(&mMutex);
}

bool ARobotBase::initialise(const char* devicePath)
{
    ScopedLock lock1(mMutex);
    if (mI2C.openSerialPort(devicePath))
    {
        mThrottlePCA.reset();
        mThrottlePCA.setFrequency(1600);
        return true;
    }
    return false;
}

float ARobotBase::getSteering() const
{
    ScopedLock lock(mMutex);
    return mSteering;
}

float ARobotBase::getThrottle() const
{
    ScopedLock lock(mMutex);
    return mThrottle;
}

float ARobotBase::getSteeringGain() const
{
    ScopedLock lock(mMutex);
    return mSteeringGain;
}

void ARobotBase::setSteeringGain(const float steeringGain)
{
    ScopedLock lock(mMutex);
    mSteeringGain = clip(steeringGain);
}

float ARobotBase::getSteeringOffset() const
{
    ScopedLock lock(mMutex);
    return mSteeringOffset;
}

void ARobotBase::setSteeringOffset(const float steeringOffset)
{
    ScopedLock lock(mMutex);
    mSteeringOffset = steeringOffset;
}

float ARobotBase::getThrottleGain() const
{
    ScopedLock lock(mMutex);
    return mThrottleGain;
}

void ARobotBase::setThrottleGain(const float throttleGain)
{
    ScopedLock lock(mMutex);
    mThrottleGain = clip(throttleGain);
}

float ARobotBase::clip(const float value)
{
    return fmaxf(-1.0f, fminf(1.0f, value));
}

bool ARobotBase::checkValue(const float newValue, const float oldValue)
{
    return std::signbit(newValue) != std::signbit(oldValue) && std::abs(newValue) > std::numeric_limits<float>::epsilon();
}