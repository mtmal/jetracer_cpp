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

#ifndef NVIDIARACER_H_
#define NVIDIARACER_H_

#include "I2C.h"
#include "PCA9685.h"
#include "Racer.h"

class NvidiaRacer: public Racer
{
public:
	NvidiaRacer();
	virtual ~NvidiaRacer();

	/**
	 * Initialises the class by opening serial port to I2C device and resetting PCA9685 boards.
	 *  @param devicePath path to I2C device, e.g. "/dev/i2c-1"
	 *  @return true if the initialisation was successful.
	 */
	bool initialise(const char* devicePath = "/dev/i2c-1");

	virtual void setSteering(const float steering);
	virtual void setThrottle(const float throttle);

private:
	float mSteeringGain;
	float mSteeringOffset;
	int mSteeringChannel;
	float mThrottleGain;
	I2C mI2C;
	PCA9685 mThrottlePCA;
	PCA9685 mSteeringPCA;
};

#endif /* NVIDIARACER_H_ */
