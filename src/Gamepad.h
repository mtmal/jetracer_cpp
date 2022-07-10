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

#ifndef GAMEPAD_H_
#define GAMEPAD_H_

class NvidiaRacer;

class Gamepad
{
public:
	/**
	 * Basic constructor, only initialises variables.
	 *  @param stopButton button ID which should be used to stop gamepad event loop.
	 *  @param controlAxis axis ID used to control robot.
	 */
	Gamepad(const int stopButton = 0, const int controlAxis = 0);

	/**
	 * Basic destructor, closes joystick device.
	 */
	virtual ~Gamepad();

	/**
	 * Initialises gamepad by opening device.
	 *  @param device path to joystick device.
	 *  @return true if connection was established.
	 */
	bool initialise(const char* device = "/dev/input/js0");

	/**
	 * Runs the main gamepad event loop and controls the racer.
	 *  @param racer the racer class which steering and throttle will be controlled.
	 */
	void runEventLoop(NvidiaRacer& racer);

private:
	/**
	 * Reads a joystick event from the joystick device.
	 *  @param[out] event data read from the joystick.
	 *  @return true on success.
	 */
	bool readEvent(struct js_event* event) const;

	/** Device ID. */
	int mDevice;
	/** Stop button number. */
	int mStopButton;
	/** Axis used to control robot. */
	int mControlAxis;
};

#endif /* GAMEPAD_H_ */
