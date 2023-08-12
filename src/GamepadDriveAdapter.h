////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2023 Mateusz Malinowski
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

#include <GenericTalker.h>
#include "DriveCommands.h"
#include "GamepadEventData.h"


/**
 * An adapter class which listens to gamepad updates and converts them into drive commands.
 */
class GamepadDriveAdapter : public IGenericListener<GamepadEventData>,
                            public GenericTalker<DriveCommands>
{
public:
    /**
     * Basic constructor, initialises control axes.
     *  @param steeringAxis ID of the gamepad axis controlling steering angle.
     *  @param throttleAxis ID of the gamepad axis controlling throttle.
     */
    GamepadDriveAdapter(const int steeringAxis = 0, const int throttleAxis = 1);

    /**
     * Basic destructor. 
     */
    virtual ~GamepadDriveAdapter();

    /**
     * Receives updates from the gamepad and converts them into drive commands.
     *  @param eventData as received from a gamepad.
     */
    void update(const GamepadEventData& eventData) override;

private:
    /** ID of the gamepad axis controlling steering angle. */
    int mSteeringAxis;
    /** ID of the gamepad axis controlling throttle. */
    int mThrottleAxis;
    /** Drive commands for broadcasting. */
    DriveCommands mDriveCommand;
};
