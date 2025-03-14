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
#include "pridopia_car.h"


PridopiaCar::PridopiaCar(const float steeringGain, const float steeringOffset, const float throttleGain)
: ARobotBase("PridopiaCar", steeringGain, steeringOffset, throttleGain)
{
}

PridopiaCar::~PridopiaCar()
{
    setSteering(0.0f);
    setThrottle(0.0f);
    mThrottlePCA.setGPIO(2, false);
    mThrottlePCA.setGPIO(8, false);
}

bool PridopiaCar::initialise(const char* devicePath)
{
    ScopedLock lock1(mMutex);
    bool flag = ARobotBase::initialise(devicePath);
    if (flag)
    {
        mThrottlePCA.setGPIO(2, true);
        mThrottlePCA.setGPIO(8, true);
    }
    return flag;
}

void PridopiaCar::setSteering(const float steering)
{
    ScopedLock lock(mMutex);
    if (checkValue(steering * mSteeringGain, mSteering))
    {
        commandWheels(0.0, 0.0);
    }

    mSteering = clip(steering) * mSteeringGain;
    commandWheels(mThrottle, mSteering);
}

void PridopiaCar::setThrottle(const float throttle)
{
    ScopedLock lock(mMutex);
    if (checkValue(throttle * mThrottleGain, mThrottle))
    {
        commandWheels(0.0, 0.0);
    }

    mThrottle = clip(throttle) * mThrottleGain;
    commandWheels(mThrottle, mSteering);
}

void PridopiaCar::update(const DriveCommands& driveCommands)
{
    ScopedLock lock(mMutex);
    if (checkValue(driveCommands.mThrottle * mThrottleGain, mThrottle) ||
        checkValue(driveCommands.mSteering * mSteeringGain, mSteering))
    {
        commandWheels(0.0, 0.0);
    }
    mSteering = clip(driveCommands.mSteering) * mSteeringGain;
    mThrottle = clip(driveCommands.mThrottle) * mThrottleGain;
    commandWheels(mThrottle, mSteering);
}

void PridopiaCar::commandWheels(const float throttle, const float steering) const
{
    float angle = std::atan2(steering, throttle) + mSteeringOffset;
    float magnitude = clip(std::sqrt(throttle * throttle + steering * steering));
    float left  = magnitude * std::cos(angle - M_PI / 4);
    float right = magnitude * std::cos(angle + M_PI / 4);
    float maxVal = std::max(std::abs(left), std::abs(right));

    if (maxVal > 1.0f)
    {
        left /= maxVal;
        right /= maxVal;
    }

    // motor 1 (8, 9, 10)
    if (left > 0)
    {
        mThrottlePCA.setDutyCycle( 9, static_cast<uint16_t>(left *  0x0FFF + 0.5f));
        mThrottlePCA.setGPIO(10, false);
    }
    else if (left < 0)
    {
        mThrottlePCA.setGPIO(9, false);
        mThrottlePCA.setDutyCycle(10, static_cast<uint16_t>(left * -0x0FFF + 0.5f));
    }
    else
    {
        mThrottlePCA.setGPIO( 9, false);
        mThrottlePCA.setGPIO(10, false);
    }

    // motor 3 (2, 3, 4)
    if (right > 0)
    {
        mThrottlePCA.setDutyCycle( 3, static_cast<uint16_t>(right *  0x0FFF + 0.5f));
        mThrottlePCA.setGPIO( 4, false);
    }
    else if (right < 0)
    {
        mThrottlePCA.setGPIO(3, false);
        mThrottlePCA.setDutyCycle( 4, static_cast<uint16_t>(right * -0x0FFF + 0.5f));
    }
    else
    {
        mThrottlePCA.setGPIO( 3, false);
        mThrottlePCA.setGPIO( 4, false);
    }
}
