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

#pragma once

#include <generic_listener.h>
#include <i2c.h>
#include "drive_commands.h"
#include "motor_controller/pca9685.h"

#define PCA9685_ADDRESS_1    0x40
#define PCA9685_ADDRESS_2    0x60

class ARobotBase : public GenericListener<DriveCommands>
{
public:
    /**
     * Basic constructor, initialises only variables.
     *  @param name the name of the robot
     *  @param steeringGain initial steering gain
     *  @param steeringOffset initial steering offset
     *  @param throttleGain initial throttle gain
     */
    ARobotBase(const char* name = "default", const float steeringGain = -0.65f, const float steeringOffset = 0, const float throttleGain = 0.8f);

    /**
     * Class destructor, set steering and throttle to zero.
     */
    virtual ~ARobotBase();

    /**
     * Initialises the class by opening serial port to I2C device and resetting PCA9685 boards.
     *  @param devicePath path to I2C device, e.g. "/dev/i2c-1"
     *  @return true if the initialisation was successful.
     */
     virtual bool initialise(const char* devicePath = "/dev/i2c-1");

    /**
     *  @return the name of the robot
     */
    inline const char* getName() const
    {
        return mName;
    }

    /**
     *  @return the steering value.
     */
    float getSteering() const;

    /**
     *  @param steering sets the new steering value.
     */
    virtual void setSteering(const float steering) = 0;

    /**
     *  @return the throttle value.
     */
    float getThrottle() const;

    /**
     *  @param steering sets the new throttle value.
     */
    virtual void setThrottle(const float throttle) = 0;

    /**
     *  @return steering gain
     */
    float getSteeringGain() const;

    /**
     *  @param steeringGain new steering gain
     */
    void setSteeringGain(const float steeringGain);

    /**
     *  @return steering offset
     */
    float getSteeringOffset() const;

    /**
     *  @param steeringOffset new steering offset
     */
    void setSteeringOffset(const float steeringOffset);

    /**
     *  @return throttle gain
     */
    float getThrottleGain() const;

    /**
     *  @param throttleGain new throttle gain
     */
    void setThrottleGain(const float throttleGain);

protected:
    /**
     *  @return clipped @p value so that it is from within -1 and 1.
     */
    static float clip(const float value);

    /**
     * Checks if the @p newValue is of the opposite sign than the @p oldValue, and whether @p newValue is non-zero.
     *  @param newValue
     *  @param oldValue
     */
    static bool checkValue(const float newValue, const float oldValue);

    /** The name of the robot. */
    const char* mName;
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
    /** Mutex for accessing values. */
    mutable pthread_mutex_t mMutex;
};
