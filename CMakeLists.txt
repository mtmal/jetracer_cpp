cmake_minimum_required(VERSION 3.10)

# set the project name
project(JetRacer_Cpp)

set(CMAKE_CXX_FLAGS "-fPIC -g -pedantic -Wall -Wextra")

if(JETRACER_PRO) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DJETRACER_PRO=1")
endif()

# add the jetracer utils submodule
if (CUSTOM_JETRACER_UTILS)
    include_directories(${CUSTOM_JETRACER_UTILS}/src)
else()
    add_subdirectory(JetracerUtils)
    include_directories(JetracerUtils/src)
    set(CUSTOM_JETRACER_UTILS JetracerUtils) # for the GamepadController submodule
endif()

# add the I2C submodule
if (CUSTOM_I2C)
    include_directories(${CUSTOM_I2C}/src)
else()
    add_subdirectory(I2C)
    include_directories(I2C/src)
    set(CUSTOM_I2C I2C) # for the MotorController submodule
endif()

# add the gamepad submodule
if (CUSTOM_GAMEPAD)
    include_directories(${CUSTOM_GAMEPAD}/src)
else()
    add_subdirectory(GamepadController)
    include_directories(GamepadController/src)
endif()

# add the gamepad submodule
if (CUSTOM_MOTOR_CONTROLLER)
    include_directories(${CUSTOM_MOTOR_CONTROLLER}/src)
else()
    add_subdirectory(MotorController)
    include_directories(MotorController/src)
endif()

include_directories(src)

# Build the actual library
add_library(JetRacer SHARED src/NvidiaRacer.cpp src/GamepadDriveAdapter.cpp)
target_link_libraries(JetRacer I2C JetracerUtils GamepadController MotorController)

# add the test application
add_executable(test_JetRacer tests/jetracer_app.cpp)
target_link_libraries(test_JetRacer JetRacer)

# add the test application
add_executable(test_JetRacer_Gamepad tests/jetracer_gamepad.cpp)
target_link_libraries(test_JetRacer_Gamepad JetRacer)
