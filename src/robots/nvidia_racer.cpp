////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2022 Mateusz Malinowski
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
#include "nvidia_racer.h"

#define PCA9685_ADDRESS_1    0x40
#define PCA9685_ADDRESS_2    0x60

namespace
{
/**
 *  @return clipped @p value so that it is from within -1 and 1.
 */
float clip(const float value)
{
    return fmaxf(-1.0f, fminf(1.0f, value));
}
} /* end of anonymous namespace */

NvidiaRacer::NvidiaRacer(const float steeringGain, const float steeringOffset, const float throttleGain)
: mSteering(0.0f),
  mThrottle(0.0f),
  mSteeringGain(steeringGain),
  mSteeringOffset(steeringOffset),
  mThrottleGain(throttleGain),
  mI2C(),
#ifdef JETRACER_PRO
  mPCA(&mI2C, PCA9685_ADDRESS_1),
  mThrottleMotor(&mPCA, 1),
  mSteeringMotor(&mPCA, 0)
#else
  mThrottlePCA(&mI2C, PCA9685_ADDRESS_2),
  mSteeringPCA(&mI2C, PCA9685_ADDRESS_1),
  mSteeringMotor(&mSteeringPCA, 0)
#endif
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&mSteeringMutex, nullptr);
    pthread_mutex_init(&mThrottleMutex, &attr);
}

NvidiaRacer::~NvidiaRacer()
{
    setSteering(0.0f);
    setThrottle(0.0f);
    pthread_mutex_destroy(&mSteeringMutex);
    pthread_mutex_destroy(&mThrottleMutex);
}

bool NvidiaRacer::initialise(const char* devicePath)
{
    ScopedLock lock1(mSteeringMutex);
    ScopedLock lock2(mThrottleMutex);
    if (mI2C.openSerialPort(devicePath))
    {
#ifdef JETRACER_PRO
        mPCA.reset();
        mThrottleMotor.initialise();
#else
        mThrottlePCA.reset();
        mThrottlePCA.setFrequency(1600);
        mSteeringPCA.reset();
        mSteeringPCA.setFrequency(50);
#endif
        mSteeringMotor.initialise();
        return true;
    }
    return false;
}

float NvidiaRacer::getSteering() const
{
    ScopedLock lock(mSteeringMutex);
    return mSteering;
}

void NvidiaRacer::setSteering(const float steering)
{
    ScopedLock lock(mSteeringMutex);
    mSteering = clip(steering);
    mSteeringMotor.setThrottle(mSteering * mSteeringGain + mSteeringOffset);
}

float NvidiaRacer::getThrottle() const
{
    ScopedLock lock(mThrottleMutex);
    return mThrottle;
}

void NvidiaRacer::setThrottle(const float throttle)
{
    ScopedLock lock(mThrottleMutex);
    if (std::signbit(throttle) != std::signbit(mThrottle) && std::abs(throttle) > std::numeric_limits<float>::epsilon())
    {
        // we want to avoid going from positive to negative direction, and vice versa, without a stop.
        setThrottle(0); // it is ok to make a recursive call, because mThrottleMutex was configured as recursive mutex
    }

    mThrottle = clip(throttle);
#ifdef JETRACER_PRO
    mThrottleMotor.setThrottle(mThrottle * mThrottleGain);
#else
    if (mThrottle > 0)
    {
        mThrottlePCA.setDutyCycle(0, static_cast<uint16_t>(mThrottle * mThrottleGain *  0x0FFF + 0.5f));
        mThrottlePCA.setGPIO(1, true);
        mThrottlePCA.setGPIO(2, false);
        mThrottlePCA.setGPIO(3, false);
        mThrottlePCA.setDutyCycle(4, static_cast<uint16_t>(mThrottle * mThrottleGain *  0x0FFF + 0.5f));
        mThrottlePCA.setGPIO(5, false);
        mThrottlePCA.setGPIO(6, true);
        mThrottlePCA.setDutyCycle(7, static_cast<uint16_t>(mThrottle * mThrottleGain *  0x0FFF + 0.5f));
    }
    else
    {
        mThrottlePCA.setDutyCycle(0, static_cast<uint16_t>(mThrottle * mThrottleGain * -0x0FFF + 0.5f));
        mThrottlePCA.setGPIO(1, false);
        mThrottlePCA.setGPIO(2, true);
        mThrottlePCA.setDutyCycle(3, static_cast<uint16_t>(mThrottle * mThrottleGain * -0x0FFF + 0.5f));
        mThrottlePCA.setGPIO(4, false);
        mThrottlePCA.setGPIO(5, true);
        mThrottlePCA.setGPIO(6, false);
        mThrottlePCA.setDutyCycle(7, static_cast<uint16_t>(mThrottle * mThrottleGain * -0x0FFF + 0.5f));
    }
#endif
}

float NvidiaRacer::getSteeringGain() const
{
    ScopedLock lock(mSteeringMutex);
    return mSteeringGain;
}

void NvidiaRacer::setSteeringGain(const float steeringGain)
{
    ScopedLock lock(mSteeringMutex);
    mSteeringGain = clip(steeringGain);
}

float NvidiaRacer::getSteeringOffset() const
{
    ScopedLock lock(mSteeringMutex);
    return mSteeringOffset;
}

void NvidiaRacer::setSteeringOffset(const float steeringOffset)
{
    ScopedLock lock(mSteeringMutex);
    mSteeringOffset = steeringOffset;
}

float NvidiaRacer::getThrottleGain() const
{
    ScopedLock lock(mThrottleMutex);
    return mThrottleGain;
}

void NvidiaRacer::setThrottleGain(const float throttleGain)
{
    ScopedLock lock(mThrottleMutex);
    mThrottleGain = clip(throttleGain);
}

void NvidiaRacer::update(const DriveCommands& driveCommands)
{
    setSteering(driveCommands.mSteering);
    setThrottle(driveCommands.mThrottle);
}
