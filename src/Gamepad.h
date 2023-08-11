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

#pragma once

#include <atomic>
#include <GenericTalker.h>
#include "GamepadEventData.h"


class Gamepad : public GenericTalker<GamepadEventData>
{
public:
	/**
	 * Basic constructor.
	 */
	Gamepad();

	/**
	 * Basic destructor, stops the event thread and closes joystick device.
	 */
	virtual ~Gamepad();

	/**
	 * Initialises gamepad by opening device.
	 *  @param device path to joystick device.
	 *  @return true if connection was established.
	 */
	bool initialise(const char* device = "/dev/input/js0");

	/**
	 * Starts the event thread.
	 *  @return true if the thread was successfully started.
	 */
	bool startEventThread();

	/**
	 * Stops the event thread.
	 */
	void stopEventThread();

	/**
	 * Runs the main gamepad event loop.
	 */
	void runEventLoop();

private:
	/**
	 * Start a thread that reads inputs from a gamepad.
	 *  @param instance an instance of this class.
	 *  @return nullptr
	 */
	static void* startEventThread(void* instance);

	/**
	 * Reads a joystick event from the joystick device.
	 *  @param[out] event data read from the joystick.
	 *  @return true on success.
	 */
	bool readEvent(struct js_event* event) const;

	/** Device ID. */
	int mDevice;
	/** Flag indicating if the event thread should run. */
	std::atomic<bool> mRun;
	/** The event thread. */
	pthread_t mEventThread;
};
