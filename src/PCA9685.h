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

#pragma once

#include <cstdint>

class I2C;

class PCA9685
{
public:
	/**
	 * Class constructor.
	 *  @param i2c a pointer to the class handling I2C communication.
	 *  @param deviceAddress address of this PCA9685 board.
	 */
	PCA9685(const I2C* i2c, const uint8_t deviceAddress);

	/**
	 * Class destructor, resets PCA9685.
	 */
	virtual ~PCA9685();

	/**
	 * Resets PCA9685.
	 */
	void reset() const;

	/**
	 *  @return the frequency of PCA9685 in Hz.
	 */
	float getFrequency() const;

	/**
	 * Sets new frequency to PCA9685.
	 *  @param frequency new frequency in Hz.
	 */
	void setFrequency(const float frequency) const;

	/** 
	 * 12 bit value that dictates how much of one cycle is high (1) versus low (0). 0x0FFF will
	 * always be high, 0 will always be low and 0x07FF will be half high and then half low. Note,
	 * if aiming for controlling the pins as GPIO, use the other method.
	 *  @param channel the channel to control (0-15).
	 *  @param value ratio of how much of the tick should start with high state.
	 */
	inline void setDutyCycle(const uint8_t channel, const uint16_t value) const
	{
		setPWM(channel, 0, value & 0x0FFF);
	}

	/**
	 * Returns the current duty cycle for a given @p channel.
	 *  @param channel the channel to query (0-15).
	 *  @return a 12-bit value.
	 */
	uint16_t getDutyCycle(const uint8_t channel) const;

	/**
	 * Sets a pin to be either fully on or fully off. 
	 *  @param channel the channel to control (0-15).
	 *  @param on true to set the @p channel full on.
	 */
	inline void setGPIO(const uint8_t channel, const bool on) const
	{
		setPWM(channel, 0x1000 * on, 0x1000 * !on);
	}

private:
	/**
	 * Returns duty cycle's on and off counts.
	 *  @param channel PCA9685's channel for which to read the information.
	 *  @param[out] on a count when the PWM duty cycle should be set to ON.
	 *  @param[out] off a count when the PWM duty cycle should be set to OFF.
	 */
	void getPWM(const uint8_t channel, uint16_t& on, uint16_t& off) const;

	/**
	 * Sets the PWM duty cycle.
	 *  @param channel PCA9685's channel which should be modified.
	 *  @param on a count when the PWM duty cycle should be set to ON.
	 *  @param off a count when the PWM duty cycle should be set to OFF.
	 */
	void setPWM(const uint8_t channel, const uint16_t on, const uint16_t off) const;

	/** Pointer to the class handling I2C communication. */
	const I2C* mI2C;
	/** The address of this PCA9685. */
	const uint8_t mDeviceAddress;
};
