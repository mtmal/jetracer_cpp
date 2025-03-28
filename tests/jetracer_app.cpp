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
#include <unistd.h>
#include <robots/nvidia_racer.h>

int main()
{
    NvidiaRacer racer;
    float testValues[] = {0.5f, 1.0f, 0.5f, 0.0f, -0.5f, -1.0f, -0.5f, 0.0f};
    puts("Initialising NvidiaRacer");
    if (racer.initialise())
    {
        racer.setThrottleGain(0.3);
        puts("Testing drive motors");
        for (float throttle : testValues)
        {
            printf("Setting throttle to %.2f \n", throttle);
            racer.setThrottle(throttle);
            sleep(1);
        }

        puts("Testing steering motor");
        for (float steering : testValues)
        {
            printf("Setting steering to %.2f \n", steering);
            racer.setSteering(steering);
            sleep(1);
        }
    }
    else
    {
        puts("Failed to initialise NvidiaRacer");
    }
    return 0;
}
