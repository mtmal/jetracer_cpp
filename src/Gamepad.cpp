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

namespace
{
class ScopedLock
{
public:
	explicit ScopedLock(pthread_mutex_t& lock) : mLock(lock)
	{
		pthread_mutex_lock(&mLock);
	}
	virtual ~ScopedLock()
	{
		pthread_mutex_unlock(&mLock);
	}
private:
	pthread_mutex_t& mLock;
};
} /* end of anonymous namespace */

Gamepad::Gamepad() : mDevice(0), mRun(false), mEventThread(0)
{
	pthread_mutex_init(&mLock, nullptr);
}

Gamepad::~Gamepad()
{
	stopEventThread();
	if (mDevice >= 0)
	{
		close(mDevice);
	}
	pthread_mutex_destroy(&mLock);
}

bool Gamepad::initialise(const char* device)
{
	mDevice = open(device, O_RDONLY);
	return mDevice >= 0;
}

bool Gamepad::startEventThread()
{
	bool retFlag = !mRun;
	if (retFlag)
	{
		mRun = true;
		retFlag = (0 == pthread_create(&mEventThread, nullptr, Gamepad::startEventThread, this));
	}
	else
	{
		puts("An event thread is already running!");
	}
	return retFlag;
}

void Gamepad::stopEventThread()
{
	mRun = false;
    if (mEventThread > 0)
    {
    	pthread_join(mEventThread, nullptr);
    	mEventThread = 0;
    }
}

void Gamepad::runEventLoop()
{
	struct js_event event;
	GamepadEventData eventData;

	while (mRun)
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
}

void* Gamepad::startEventThread(void* instance)
{
    Gamepad* gamepad = static_cast<Gamepad*>(instance);
    gamepad->runEventLoop();
    return nullptr;
}

bool Gamepad::readEvent(struct js_event* event) const
{
    return read(mDevice, event, sizeof(*event)) == sizeof(*event);
}

int Gamepad::registerListener(const GamepadListener& listener)
{
	int id = rand();
	ScopedLock lock(mLock);
	mListerers.insert(std::pair<int, const GamepadListener&>(id, listener));
	return id;
}

void Gamepad::unregisterListener(const int id)
{
	std::map<int, const GamepadListener&>::iterator it;
	ScopedLock lock(mLock);
	it = mListerers.find(id);
	mListerers.erase(it);
}

void Gamepad::notifyListeners(const GamepadEventData& eventData)
{
	ScopedLock lock(mLock);
	for (const std::pair<int, const GamepadListener&>& listener : mListerers)
	{
		listener.second.update(eventData);
	}
}