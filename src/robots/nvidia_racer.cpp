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

#include "nvidia_racer.h"


NvidiaRacer::NvidiaRacer(const float steeringGain, const float steeringOffset, const float throttleGain)
: ARobotBase("NvidiaRacer", steeringGain, steeringOffset, throttleGain),
#ifdef JETRACER_PRO
  mThrottleMotor(&mThrottlePCA, 1),
  mSteeringMotor(&mThrottlePCA, 0)
#else
  mSteeringPCA(&mI2C, PCA9685_ADDRESS_1),
  mSteeringMotor(&mSteeringPCA, 0)
#endif
{
    pthread_mutex_init(&mSteeringMutex, nullptr);
}

NvidiaRacer::~NvidiaRacer()
{
    setSteering(0.0f);
    setThrottle(0.0f);
    pthread_mutex_destroy(&mSteeringMutex);
}

bool NvidiaRacer::initialise(const char* devicePath)
{
    ScopedLock lock1(mMutex);
    ScopedLock lock2(mSteeringMutex);
    bool flag = ARobotBase::initialise(devicePath);
    if (flag)
    {
#ifdef JETRACER_PRO
        mPCA.reset();
        mThrottleMotor.initialise();
#else
        mSteeringPCA.reset();
        mSteeringPCA.setFrequency(50);
#endif
        mSteeringMotor.initialise();
    }
    return flag;
}

void NvidiaRacer::setSteering(const float steering)
{
    ScopedLock lock(mSteeringMutex);
    mSteering = clip(steering);
    mSteeringMotor.setThrottle(mSteering * mSteeringGain + mSteeringOffset);
}

void NvidiaRacer::setThrottle(const float throttle)
{
    ScopedLock lock(mMutex);
    if (checkValue(throttle, mThrottle))
    {
        // we want to avoid going from positive to negative direction, and vice versa, without a stop.
        setThrottle(0); // it is ok to make a recursive call, because mMutex was configured as recursive mutex
    }

    mThrottle = clip(throttle) * mThrottleGain;
#ifdef JETRACER_PRO
    mThrottleMotor.setThrottle(mThrottle);
#else
    if (mThrottle > 0)
    {
        mThrottlePCA.setDutyCycle(0, static_cast<uint16_t>(mThrottle *  0x0FFF + 0.5f));
        mThrottlePCA.setGPIO(1, true);
        mThrottlePCA.setGPIO(2, false);
        mThrottlePCA.setGPIO(3, false);
        mThrottlePCA.setDutyCycle(4, static_cast<uint16_t>(mThrottle *  0x0FFF + 0.5f));
        mThrottlePCA.setGPIO(5, false);
        mThrottlePCA.setGPIO(6, true);
        mThrottlePCA.setDutyCycle(7, static_cast<uint16_t>(mThrottle *  0x0FFF + 0.5f));
    }
    else
    {
        mThrottlePCA.setDutyCycle(0, static_cast<uint16_t>(mThrottle * -0x0FFF + 0.5f));
        mThrottlePCA.setGPIO(1, false);
        mThrottlePCA.setGPIO(2, true);
        mThrottlePCA.setDutyCycle(3, static_cast<uint16_t>(mThrottle * -0x0FFF + 0.5f));
        mThrottlePCA.setGPIO(4, false);
        mThrottlePCA.setGPIO(5, true);
        mThrottlePCA.setGPIO(6, false);
        mThrottlePCA.setDutyCycle(7, static_cast<uint16_t>(mThrottle * -0x0FFF + 0.5f));
    }
#endif
}

void NvidiaRacer::update(const DriveCommands& driveCommands)
{
    setSteering(driveCommands.mSteering);
    setThrottle(driveCommands.mThrottle);
}
