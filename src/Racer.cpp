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
#include "Racer.h"

namespace
{
/**
 *  @return clipped @p value so that it is from within -1 and 1.
 */
float clip(const float value)
{
	return fmaxf(-1.0f, fminf(1.0f, value));
}
} /* end of anonymous namespace */

Racer::Racer() : mSteering(0.0f), mThrottle(0.0f)
{
}

Racer::~Racer()
{
	setSteering(0.0f);
	setThrottle(0.0f);
}

void Racer::setSteering(const float steering)
{
	mSteering = clip(steering);
}

void Racer::setThrottle(const float throttle)
{
	mThrottle = clip(throttle);
}
