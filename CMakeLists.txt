cmake_minimum_required(VERSION 3.14)
project(rosbridge_client)

set(cpprestsdk_DIR /usr/lib/x86_64-linux-gnu/cmake)
find_package(cpprestsdk REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(rosbridge_client source/rosbridge_client.cpp)
target_link_libraries(rosbridge_client PUBLIC cpprestsdk::cpprest)

add_executable(rosbridge_client_test source/rosbridge_client.cxx)
target_link_libraries(rosbridge_client_test PRIVATE rosbridge_client )
