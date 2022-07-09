cmake_minimum_required(VERSION 3.10)

# set the project name
project(jetracer_cpp)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-fPIC -g -pedantic -Wall -Wextra")

if(LOG) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOG=1")
endif()

include_directories(src)

# Build the actual library
add_library(jetracer SHARED src/Racer.h src/Racer.cpp src/NvidiaRacer.h src/NvidiaRacer.cpp src/PCA9685.h src/PCA9685.cpp src/I2C.h src/I2C.cpp)

# add the test application
add_executable(test_jetracer tests/jetracer_app.cpp)
target_link_libraries(test_jetracer jetracer)