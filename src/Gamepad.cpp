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
//
// This class is based on Jason White's code found in
// https://gist.github.com/jasonwhite/c5b2048c15993d285130#file-joystick-c
////////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include "Gamepad.h"
#include "NvidiaRacer.h"

static constexpr float MAX_SHORT = 32767.0f;

Gamepad::Gamepad(const int stopButton, const int controlAxis)
: mDevice(0), mStopButton(stopButton), mControlAxis(controlAxis)
{
}

Gamepad::~Gamepad()
{
	if (mDevice >= 0)
	{
		close(mDevice);
	}
}

bool Gamepad::initialise(const char* device)
{
	mDevice = open(device, O_RDONLY);
	return mDevice >= 0;
}

void Gamepad::runEventLoop(NvidiaRacer& racer)
{
	struct js_event event;
	short x, y;
	bool run = true;

	while (run && readEvent(&event))
	{
		switch (event.type)
		{
			case JS_EVENT_BUTTON:
				run = (event.number != mStopButton);
				break;
			case JS_EVENT_AXIS:
				if (getAxisState(&event, x, y))
				{
					racer.setThrottle(static_cast<float>(-y) / MAX_SHORT);
					racer.setSteering(static_cast<float>( x) / MAX_SHORT);
				}
				break;
			default:
				break;
		}
	}
	racer.setThrottle(0);
	racer.setSteering(0);
}

bool Gamepad::readEvent(struct js_event* event) const
{
    return read(mDevice, event, sizeof(*event)) == sizeof(*event);
}

bool Gamepad::getAxisState(const struct js_event* event, short& x, short& y) const
{
    if (event->number / 2 == mControlAxis)
    {
        if (event->number % 2 == 0)
        {
            x = event->value;
        }
        else
        {
            y = event->value;
        }
        return true;
    }
    return false;
}
