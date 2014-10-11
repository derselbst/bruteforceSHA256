cmake_minimum_required(VERSION 2.8)

project(bruteforceSHA256)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -DVERBOSE")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

set(LD_FLAGS "-lcrypto")

set(_SRC
	brute.cpp
)

add_executable(bruteSHA ${_SRC})

target_link_libraries(bruteSHA ${LD_FLAGS})