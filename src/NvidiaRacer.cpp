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

#include "NvidiaRacer.h"

#define PCA9685_ADDRESS_1	0x40
#define PCA9685_ADDRESS_2	0x60

NvidiaRacer::NvidiaRacer() : Racer(), mSteeringGain(-0.65f), mSteeringOffset(0.0f), mSteeringChannel(0),
							 mThrottleGain(0.8f), mI2C(), mThrottlePCA(&mI2C, PCA9685_ADDRESS_2),
							 mSteeringPCA(&mI2C, PCA9685_ADDRESS_1)
{
}

NvidiaRacer::~NvidiaRacer()
{
	setSteering(0.0f);
	setThrottle(0.0f);
}

bool NvidiaRacer::initialise(const char* devicePath)
{
	if (mI2C.openSerialPort(devicePath))
	{
		mThrottlePCA.reset();
		mThrottlePCA.setFrequency(1600);
		mSteeringPCA.reset();
		return true;
	}
	return false;
}

void NvidiaRacer::setSteering(const float steering)
{
	Racer::setSteering(steering);
}

void NvidiaRacer::setThrottle(const float throttle)
{
	Racer::setThrottle(throttle);
	if (mThrottle > 0)
	{
		mThrottlePCA.setDutyCycle(0, static_cast<uint16_t>(mThrottle * mThrottleGain * 0xFFFF));
		mThrottlePCA.setDutyCycle(1, 0xFFFF);
		mThrottlePCA.setDutyCycle(2, 0);
		mThrottlePCA.setDutyCycle(3, 0);
		mThrottlePCA.setDutyCycle(4, static_cast<uint16_t>(mThrottle * mThrottleGain * 0xFFFF));
		mThrottlePCA.setDutyCycle(7, static_cast<uint16_t>(mThrottle * mThrottleGain * 0xFFFF));
		mThrottlePCA.setDutyCycle(6, 0xFFFF);
		mThrottlePCA.setDutyCycle(5, 0);
	}
	else
	{
		mThrottlePCA.setDutyCycle(0, static_cast<uint16_t>(mThrottle * mThrottleGain * -0xFFFF));
		mThrottlePCA.setDutyCycle(1, 0);
		mThrottlePCA.setDutyCycle(2, 0xFFFF);
		mThrottlePCA.setDutyCycle(3, static_cast<uint16_t>(mThrottle * mThrottleGain * -0xFFFF));
		mThrottlePCA.setDutyCycle(4, 0);
		mThrottlePCA.setDutyCycle(7, static_cast<uint16_t>(mThrottle * mThrottleGain * -0xFFFF));
		mThrottlePCA.setDutyCycle(6, 0);
		mThrottlePCA.setDutyCycle(5, 0xFFFF);
	}
}
