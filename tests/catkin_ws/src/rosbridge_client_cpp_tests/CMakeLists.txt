cmake_minimum_required(VERSION 3.0.2)
project(rosbridge_client_cpp_tests)

## C++11 Flags
##
add_compile_options(-std=c++11)
add_compile_options(-pthread)
add_compile_options(-Werror=return-type)                   # error on missing return type
add_compile_options(-Werror=format)                        # error on wrong printf formats
add_compile_options(-Werror=parentheses)                   # error when using ambiguous syntax

find_package(catkin REQUIRED COMPONENTS roscpp std_msgs)
find_package(rostest REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../CMakeLists.txt)

message(STATUS AAAAAAAAAAAAAAA ${ROSBRIDGE_CLIENT_GTEST})

## Integration Tests
add_rostest_gtest(integration_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/integration_tests.launch
    ${CMAKE_CURRENT_SOURCE_DIR}/integration_tests.cpp
    ${ROSBRIDGE_CLIENT_GTEST}
)
target_link_libraries(integration_tests ${ROSBRIDGE_CLIENT_CPP_LIBS} ${catkin_LIBRARIES})
add_dependencies(integration_tests ${catkin_EXPORTED_TARGETS})
