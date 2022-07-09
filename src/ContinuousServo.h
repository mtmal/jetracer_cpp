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

#ifndef CONTINUOUSSERVO_H_
#define CONTINUOUSSERVO_H_

#include <cstdint>

class PCA9685;

class ContinuousServo
{
public:
	/**
	 * Class constructor, only initialises variables.
	 *  @param pca9685 pointer to PCA9685 class which provides functionality for PWM control.
	 *  @param channel a PCA9685 channel to which this servo is connected.
	 */
	ContinuousServo(const PCA9685* pca9685, const uint8_t channel = 0);

	/**
	 * Class destructor. Sets throttle to 0.
	 */
	virtual ~ContinuousServo();

	/**
	 * Initialises the class by setting minimum duty and duty range.
	 *  @param minPulse the minimum pulse width of the servo in microseconds.
	 *  @param maxPulse the maximum pulse width of the servo in microseconds.
	 */
	void initialise(const int minPulse = 750, const int maxPulse = 2250);

	/**
	 * Sets the throttle of the servo.
	 *  @param throttle a value from -1 to 1.
	 */
	inline void setThrottle(const float throttle) const
	{
		setFraction((throttle + 1) / 2);
	}

	/**
	 *  @return current servo throttle from -1 to 1.
	 */
	inline float getThrottle() const
	{
		return getFraction() * 2 - 1;
	}

private:
	/** Pulse width expressed as fraction between 0.0 (`minPulse`) and 1.0 (`maxPulse`).
        For conventional servos, corresponds to the servo position as a fraction
        of the actuation range. */
	void setFraction(const float fraction) const;
	float getFraction() const;

	/** Pointer to PCA9685 class for PWM control functionality. */
	const PCA9685* mPCA9685;
	/** A PCA9685 channel to which this servo is connected. */
	uint8_t mChannel;
	/** A minimum PWM duty for steering control. */
	uint16_t mMinDuty;
	/** Duty range for the whole steering range. */
	float mDutyRange;
};

#endif /* CONTINUOUSSERVO_H_ */
