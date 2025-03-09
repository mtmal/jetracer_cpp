cmake_minimum_required(VERSION 3.10)

# set the project name
project(RobotController)

set(CMAKE_CXX_FLAGS "-fPIC -g -pedantic -Wall -Wextra")

if(JETRACER_PRO) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DJETRACER_PRO=1")
endif()

# add the jetracer utils submodule
if (CUSTOM_UTILS)
    include_directories(${CUSTOM_UTILS}/src)
else()
    add_subdirectory(Utils)
    include_directories(Utils/src)
    set(CUSTOM_UTILS JetracerUtils) # for the Gamepad submodule
endif()

# add the I2C submodule
if (CUSTOM_I2C)
    include_directories(${CUSTOM_I2C}/src)
else()
    add_subdirectory(I2C)
    include_directories(I2C/src)
endif()

# add the gamepad submodule
if (CUSTOM_GAMEPAD)
    include_directories(${CUSTOM_GAMEPAD}/src)
else()
    add_subdirectory(GamepadController)
    include_directories(GamepadController/src)
endif()

include_directories(src)

# Build the actual library
add_library(RobotController SHARED src/robots/nvidia_racer.cpp src/motor_controller/pca9685.cpp src/motor_controller/continuous_servo.cpp src/gamepad_drive_adapter.cpp)
target_link_libraries(RobotController I2C GamepadController pthread)

# add the test application
add_executable(test_jestracer tests/jetracer_app.cpp)
target_link_libraries(test_jestracer RobotController)

# add the test application
add_executable(test_jestracer_gamepad tests/jetracer_gamepad.cpp)
target_link_libraries(test_jestracer_gamepad RobotController)
