cmake_minimum_required(VERSION 3.10)

# set the project name
project(JetRacer_Cpp)

set(CMAKE_CXX_FLAGS "-fPIC -g -pedantic -Wall -Wextra")

if(JETRACER_PRO) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DJETRACER_PRO=1")
endif()

if(CUSTOM_JETRACER_UTILS AND CUSTOM_I2C)
    include(CMakePrintHelpers)
    cmake_print_variables(CUSTOM_JETRACER_UTILS)
    include_directories(${CUSTOM_JETRACER_UTILS}/src)

    cmake_print_variables(CUSTOM_I2C)
    include_directories(${CUSTOM_I2C}/src)
else()
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

    # add the I2C submodule
    add_subdirectory(I2C)
    include_directories(I2C/src)

    # add the jetracer utils submodule
    add_subdirectory(JetracerUtils)
    include_directories(JetracerUtils/src)
endif()

include_directories(src)

# Build the actual library
add_library(JetRacer SHARED src/NvidiaRacer.cpp src/PCA9685.cpp src/ContinuousServo.cpp src/Gamepad.cpp src/GamepadDriveAdapter.cpp)
target_link_libraries(JetRacer I2C JetracerUtils)

# add the test application
add_executable(test_JetRacer tests/jetracer_app.cpp)
target_link_libraries(test_JetRacer JetRacer)

# add the test application
add_executable(test_JetRacer_Gamepad tests/jetracer_gamepad.cpp)
target_link_libraries(test_JetRacer_Gamepad JetRacer)
