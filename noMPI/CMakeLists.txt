cmake_minimum_required(VERSION 2.8)

project(bruteforceSHA256)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -DVERBOSE")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

find_package(OpenSSL REQUIRED)
set(LD_FLAGS ${LD_FLAGS} ${OPENSSL_LIBRARIES})
message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

set(_SRC
	brute.cpp
	test.cpp
)

add_executable(bruteSHA ${_SRC})

target_link_libraries(bruteSHA ${LD_FLAGS})