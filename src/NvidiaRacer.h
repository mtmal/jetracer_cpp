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

#include "ContinuousServo.h"
#include "I2C.h"
#include "PCA9685.h"

class NvidiaRacer
{
public:
	/**
	 * Basic constructor, initialises only variables.
	 *  @param steeringGain initial steering gain
	 *  @param steeringOffset initial steering offset
	 *  @param throttleGain initial throttle gain
	 */
	NvidiaRacer(const float steeringGain = -0.65f, const float steeringOffset = 0, const float throttleGain = 0.8f);

	/**
	 * Class destructor, set steering and throttle to zero.
	 */
	virtual ~NvidiaRacer();

	/**
	 * Initialises the class by opening serial port to I2C device and resetting PCA9685 boards.
	 *  @param devicePath path to I2C device, e.g. "/dev/i2c-1"
	 *  @return true if the initialisation was successful.
	 */
	bool initialise(const char* devicePath = "/dev/i2c-1");

	/**
	 *  @return the steering value.
	 */
	inline float getSteering() const
	{
		return mSteering;
	}

	/**
	 *  @param steering sets the new steering value.
	 */
	void setSteering(const float steering);

	/**
	 *  @return the throttle value.
	 */
	inline float getThrottle() const
	{
		return mThrottle;
	}

	/**
	 *  @param steering sets the new throttle value.
	 */
	void setThrottle(const float throttle);

	/**
	 *  @return steering gain
	 */
	inline float getSteeringGain() const
	{
		return mSteeringGain;
	}

	/**
	 *  @param steeringGain new steering gain
	 */
	inline void setSteeringGain(const float steeringGain)
	{
		mSteeringGain = steeringGain;
	}

	/**
	 *  @return steering offset
	 */
	inline float getSteeringOffset() const
	{
		return mSteeringOffset;
	}

	/**
	 *  @param steeringOffset new steering offset
	 */
	inline void setSteeringOffset(const float steeringOffset)
	{
		mSteeringOffset = steeringOffset;
	}

	/**
	 *  @return throttle gain
	 */
	inline float getThrottleGain() const
	{
		return mThrottleGain;
	}

	/**
	 *  @param throttleGain new throttle gain
	 */
	inline void setThrottleGain(const float throttleGain)
	{
		mThrottleGain = throttleGain;
	}

private:
	/** Steering control for the racer, value from -1 to 1. */
	float mSteering;
	/** Throttle control for the racer, value from -1 to 1. */
	float mThrottle;
	/** Steering gain for steering control. */
	float mSteeringGain;
	/** A steering offset for steering wheels alignment. */
	float mSteeringOffset;
	/** Throttle gain for throttle control. */
	float mThrottleGain;
	/** Object for I2C communication. */
	I2C mI2C;
	/** PCA9685 board which controls drive motors. */
	PCA9685 mThrottlePCA;
	/** PCA9685 board which controls steering motor. */
	PCA9685 mSteeringPCA;
	/** Object for controlling steering motor. */
	ContinuousServo mServo;
};

#endif /* NVIDIARACER_H_ */
