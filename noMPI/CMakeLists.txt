cmake_minimum_required(VERSION 2.8)

project(bruteforceSHA256)

set(CMAKE_C_FLAGS "-g -Wall -Wextra")
set(LD_FLAGS "")

set(_SRC
	brute.cpp
)

add_executable(bruteSHA ${_SRC})

target_link_libraries(bruteSHA ${LD_FLAGS})