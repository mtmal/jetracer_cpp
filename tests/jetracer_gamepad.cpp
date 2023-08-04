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

#include <cstdio>
#include <semaphore.h>
#include <unistd.h>
#include <Gamepad.h>
#include <NvidiaRacer.h>


constexpr float MAX_SHORT = 32767.0f;

class ControlCar : public IGenericListener<GamepadEventData>
{
public:
	ControlCar(sem_t* semaphore, NvidiaRacer& racer, Gamepad& gamepad, int stopButton = 0, int controlAxis = 0)
	: mSemaphore(semaphore), 
	  mRacer(racer), 
	  mGamepad(gamepad), 
	  mStopButton(stopButton), 
	  mControlAxis(controlAxis)
	{
		mListenerId = mGamepad.registerListener(*this);
	}

	virtual ~ControlCar()
	{
		mGamepad.unregisterListener(mListenerId);
	}

	void update(const GamepadEventData& eventData) override
	{
		if (eventData.mIsAxis)
		{
			if (eventData.mNumber / 2 == mControlAxis)
			{
				if (eventData.mNumber % 2 == 0)
				{
					mRacer.setSteering(static_cast<float>(-eventData.mValue) / MAX_SHORT);
				}
				else
				{
					mRacer.setThrottle(static_cast<float>( eventData.mValue) / MAX_SHORT);
				}
			}
		}
		else
		{
			if (eventData.mNumber == mStopButton)
			{
				mGamepad.stopEventThread();
				sem_post(mSemaphore);
			}
		}
	}

private:
	/** Pointer to semaphore that should be notified when stopping the application. */
	sem_t* mSemaphore;
	/** Reference to the Nvidia Racer class. */
	NvidiaRacer& mRacer;
	/** Reference to the gamepad class. */
	Gamepad& mGamepad;
	/** Stop button number. */
	int mStopButton;
	/** Axis used to control robot. */
	int mControlAxis;
	/** Listener ID. */
	int mListenerId;
};

int main()
{
	sem_t semaphore;
	NvidiaRacer racer;
	Gamepad gamepad;
	ControlCar controlCar(&semaphore, racer, gamepad);
	sem_init(&semaphore, 0, 0);
	puts("Initialising NvidiaRacer");
	if (racer.initialise())
	{
		racer.setThrottleGain(0.5);
		puts("Initialising Gamepad");
		if (gamepad.initialise())
		{
			puts("Starting event loop");
			if (gamepad.startEventThread())
			{
				puts("Event loop started.");
				while (0 == sem_wait(&semaphore))
				{
					;
				}
			}
		}
		else
		{
			puts("Failed to initialise Gamepad");
		}
	}
	else
	{
		puts("Failed to initialise NvidiaRacer");
	}
	puts("Finished");
    return 0;
}


