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
#include <unistd.h>
#include "I2C.h"
#include "PCA9685.h"

#define MODE1              0x00
#define MODE2              0x01
#define SUBADR1            0x02
#define SUBADR2            0x03
#define SUBADR3            0x04
#define PRESCALE           0xFE
#define LED0_ON_L          0x06
#define LED0_ON_H          0x07
#define LED0_OFF_L         0x08
#define LED0_OFF_H         0x09
#define ALL_LED_ON_L       0xFA
#define ALL_LED_ON_H       0xFB
#define ALL_LED_OFF_L      0xFC
#define ALL_LED_OFF_H      0xFD

#define RESTART            0x80
#define SLEEP              0x10
#define ALLCALL            0x01
#define INVRT              0x10
#define OUTDRV             0x04
#define AUTO_INCR          0x20

static constexpr float REFERENCE_CLK_SPEED_SCALED = static_cast<float>(25000000.0 / 4096.0);

PCA9685::PCA9685(const I2C* i2c, const uint8_t deviceAddress) : mI2C(i2c), mDeviceAddress(deviceAddress)
{
}

PCA9685::~PCA9685()
{
	reset();
}

void PCA9685::reset() const
{
	mI2C->writeByte(mDeviceAddress, MODE1, 0);
}

float PCA9685::getFrequency() const
{
	return REFERENCE_CLK_SPEED_SCALED / static_cast<float>(mI2C->readByte(mDeviceAddress, PRESCALE));
}

void PCA9685::setFrequency(const float frequency) const
{
	uint8_t oldMode;
	uint8_t prescale = static_cast<uint8_t>(std::round(REFERENCE_CLK_SPEED_SCALED / frequency)) - 1;
	if (prescale >= 3)
	{
		oldMode = mI2C->readByte(mDeviceAddress, MODE1); // Mode 1
		mI2C->writeByte(mDeviceAddress, MODE1, (oldMode & 0x7F) | SLEEP); // Mode 1, sleep
		mI2C->writeByte(mDeviceAddress, PRESCALE, prescale); // Prescale
		mI2C->writeByte(mDeviceAddress, MODE1, oldMode); // Mode 1

		usleep(5000);
		// Mode 1, autoincrement on, fix to stop pca9685 from accepting commands at all addresses
		mI2C->writeByte(mDeviceAddress, MODE1, oldMode | RESTART | AUTO_INCR);
	}
}

uint16_t PCA9685::getDutyCycle(const uint8_t channel) const
{
	uint16_t on, off;
	getPWM(channel, on, off);
	if (on == 0x1000)
	{
		return 0xFFFF;
	}
	else
	{
		return off << 4;
	}
}

void PCA9685::setDutyCycle(const uint8_t channel, const uint16_t value) const
{
	if (value == 0xFFFF)
	{
		setPWM(channel, 0x1000, 0);
	}
	else
	{
		setPWM(channel, 0, (value + 1) >> 4);
	}
}

void PCA9685::getPWM(const uint8_t channel, uint16_t& on, uint16_t& off) const
{
	// TODO: make a block read once tested.
	uint8_t channelOffset = 4 * channel;
	uint8_t on_l  = mI2C->readByte(mDeviceAddress, LED0_ON_L  + channelOffset);
	uint8_t on_h  = mI2C->readByte(mDeviceAddress, LED0_ON_H  + channelOffset);
	uint8_t off_l = mI2C->readByte(mDeviceAddress, LED0_OFF_L + channelOffset);
	uint8_t off_h = mI2C->readByte(mDeviceAddress, LED0_OFF_H + channelOffset);
	on  = (static_cast<uint16_t>(on_h)  << 8) & on_l;
	off = (static_cast<uint16_t>(off_h) << 8) & off_l;
}

void PCA9685::setPWM(const uint8_t channel, const uint16_t on, const uint16_t off) const
{
	// TODO: make a block write once tested.
	uint8_t channelOffset = 4 * channel;
	mI2C->writeByte(mDeviceAddress, LED0_ON_L  + channelOffset, on  &  0xFF);
	mI2C->writeByte(mDeviceAddress, LED0_ON_H  + channelOffset, on  >>  8);
	mI2C->writeByte(mDeviceAddress, LED0_OFF_L + channelOffset, off &  0xFF);
	mI2C->writeByte(mDeviceAddress, LED0_OFF_H + channelOffset, off >>  8);
}
