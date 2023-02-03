# C++ Implementation of JetRacer
A C++ implementation of Waveshare's JetRacer platform. It uses my own implementation I2C communication which is done in a separate repository (CMake will pull it). There are two test applications. One te$

The code has been tested on Jetson Nano with JetPack 4.5.1.

## Setup
Default build without internal logging
```
$ mkdir build
$ cd build
$ cmake ..
$ make     
$ ./test_JetRacer        
```

## Using Gamepad
To drive using provided PS3 gamepad, simply plug its receiver into one of Jetson's USB port (this is different compared to vanilla Python imlpementations where commands were sent from host) and run
```
$ ./test_JetRacer_Gamepad 
```

Note that you may need to use gamepad's HOME button to switch between modes. By default, button with ID 0 is used to stop the application and axis with ID 0 is used to control JetRacer (forward, backward, and seteering).
