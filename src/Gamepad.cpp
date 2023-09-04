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

#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include "Gamepad.h"
#include "NvidiaRacer.h"

Gamepad::Gamepad() 
: GenericTalker<GamepadEventData>(),
  GenericThread<Gamepad>(),
  mDevice(0)
{
}

Gamepad::~Gamepad()
{
	stopThread();
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

void* Gamepad::theadBody()
{
	struct js_event event;
	GamepadEventData eventData;

	while (isRunning())
	{
		if (readEvent(&event))
		{
			switch (event.type)
			{
				case JS_EVENT_BUTTON:
					// fall-through
				case JS_EVENT_AXIS:
					eventData.mIsAxis = (JS_EVENT_AXIS == event.type);
					eventData.mNumber = event.number;
					eventData.mValue = event.value;
					notifyListeners(eventData);
					break;
				default:
					break;
			}
		}
		else
		{
			puts("Failed to process a gamepad event!");
		}
		usleep(10000);
	}
	return nullptr;
}

bool Gamepad::readEvent(struct js_event* event) const
{
    return read(mDevice, event, sizeof(*event)) == sizeof(*event);
}