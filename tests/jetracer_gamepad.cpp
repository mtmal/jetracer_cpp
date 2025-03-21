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
#include <cstring>
#include <semaphore.h>
#include <unistd.h>
#include <gamepad.h>
#include <gamepad_drive_adapter.h>
#include <robots/nvidia_racer.h>
#include <robots/pridopia_car.h>


constexpr float MAX_SHORT = 32767.0f;

class ControlCar : public GenericListener<GamepadEventData>
{
public:
    ControlCar(int stopButton = 0) : mStopButton(stopButton)
    {
        sem_init(&mSemaphore, 0, 0);
    }

    virtual ~ControlCar()
    {
        sem_destroy(&mSemaphore);
    }

    inline sem_t* getSem()
    {
        return &mSemaphore;
    }

    void update(const GamepadEventData& eventData) override
    {
        if (eventData.mIsAxis == 0 && eventData.mNumber == mStopButton)
        {
            sem_post(&mSemaphore);
        }
    }

private:
    /** Pointer to semaphore that should be notified when stopping the application. */
    sem_t mSemaphore;
    /** Stop button number. */
    int mStopButton;
};

int main(int argc, char** argv)
{
    ARobotBase* robot = nullptr;
    
    if (argc != 4)
    {
        printf("Usage: %s <nvidia | pridopia> <throttle gain> <steering offset> \n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "nvdia") == 0)
    {
        robot = new NvidiaRacer();
    }
    else if (strcmp(argv[1], "pridopia") == 0)
    {
        robot = new PridopiaCar();
    }
    else
    {
        printf("Unknown robot %s. Use either nvdia or pridopia \n", argv[1]);
        return 2;
    }

    printf("Initialising %s \n", robot->getName());
    if (robot->initialise())
    {
        Gamepad gamepad;
        GamepadDriveAdapter adapter;
        robot->setThrottleGain(atof(argv[2]));
        robot->setSteeringOffset(atof(argv[3]));
        static_cast<GenericTalker<DriveCommands>&>(adapter).registerTo(robot);
        puts("Initialising Gamepad");
        if (gamepad.initialise())
        {
            ControlCar controlCar;
            gamepad.registerTo(&controlCar);
            gamepad.registerTo(static_cast<GenericListener<GamepadEventData>*>(&adapter));
            puts("Starting event loop");
            if (gamepad.startThread())
            {
                puts("Event loop started.");
                while (0 != sem_wait(controlCar.getSem()))
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
        printf("Failed to initialise %s \n", robot->getName());
    }
    puts("Finished");
    return 0;
}
