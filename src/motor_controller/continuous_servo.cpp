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

#include "continuous_servo.h"
#include "pca9685.h"

static constexpr float OFFSET = 0x0FFF / 1000000.0f;


ContinuousServo::ContinuousServo(const PCA9685* pca9685, const uint8_t channel)
: mPCA9685(pca9685), 
  mChannel(channel), 
  mMinDuty(0), 
  mDutyRange(0)
{
}

ContinuousServo::~ContinuousServo()
{
    setThrottle(0.0f);
}

void ContinuousServo::initialise(const int minPulse, const int maxPulse)
{
    float frequency = mPCA9685->getFrequency();
    float maxDuty   = static_cast<float>(maxPulse) * frequency * OFFSET;
    mMinDuty        = static_cast<float>(minPulse) * frequency * OFFSET;
    mDutyRange         = maxDuty - mMinDuty;
}

void ContinuousServo::setFraction(const float fraction) const
{
    mPCA9685->setDutyCycle(mChannel, static_cast<uint16_t>(mMinDuty + fraction * mDutyRange + 0.5f));
}

float ContinuousServo::getFraction() const
{
    return static_cast<float>(mPCA9685->getDutyCycle(mChannel) - mMinDuty) / mDutyRange;
}
