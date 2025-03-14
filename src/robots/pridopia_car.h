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

#include "abstract_robot_base.h"

class PridopiaCar : public ARobotBase
{
public:
    /**
     * Basic constructor, initialises only variables.
     *  @param steeringGain initial steering gain
     *  @param steeringOffset initial steering offset
     *  @param throttleGain initial throttle gain
     */
    PridopiaCar(const float steeringGain = 1.0f, const float steeringOffset = 0, const float throttleGain = 0.8f);

    /**
     * Class destructor, set steering and throttle to zero.
     */
    virtual ~PridopiaCar();

    bool initialise(const char* devicePath = "/dev/i2c-1") override;
    void setSteering(const float steering) override;
    void setThrottle(const float throttle) override;
    void update(const DriveCommands& driveCommands) override;

private:
    /**
     * Commands the wheels by conveting @p throttle and @p steering to individual wheel commands.
     *  @param throttle
     *  @param steering
     */
    void commandWheels(const float throttle, const float steering) const;
};
