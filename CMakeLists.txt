cmake_minimum_required(VERSION 3.10)

# set the project name
project(JetRacer_Cpp)

set(CMAKE_CXX_FLAGS "-fPIC -g -pedantic -Wall -Wextra")

# make sure we have the necessary submodule
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/I2C/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

include_directories(src)

# add the I2C submodule
add_subdirectory(I2C)
include_directories(I2C/src)

# add the jetracer utils submodule
add_subdirectory(JetracerUtils)
include_directories(JetracerUtils/src)

# Build the actual library
add_library(JetRacer SHARED src/NvidiaRacer.h src/NvidiaRacer.cpp src/PCA9685.h src/PCA9685.cpp src/ContinuousServo.h src/ContinuousServo.cpp src/Gamepad.h src/Gamepad.cpp)
target_link_libraries(JetRacer I2C JetracerUtils)

# add the test application
add_executable(test_JetRacer tests/jetracer_app.cpp)
target_link_libraries(test_JetRacer JetRacer)

# add the test application
add_executable(test_JetRacer_Gamepad tests/jetracer_gamepad.cpp)
target_link_libraries(test_JetRacer_Gamepad JetRacer)
