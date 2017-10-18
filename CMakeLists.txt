cmake_minimum_required(VERSION 3.0.2)
project(rosbridge_client_cpp)
set(rosbridge_client_cpp_VERSION_MAJOR 0)
set(rosbridge_client_cpp_VERSION_MINOR 1)
set(rosbridge_client_cpp_VERSION_PATCH 0)

## C++11 Flags
##
add_compile_options(-std=c++11)
add_compile_options(-pthread)
add_compile_options(-Werror=return-type)                   # error on missing return type
add_compile_options(-Werror=format)                        # error on wrong printf formats
add_compile_options(-Werror=parentheses)                   # error when using ambiguous syntax
add_compile_options(-Werror=reorder)                       # error on wrong constructor initialization order

#### CONFIG ############################################################################################################

set(ROSBRIDGE_CLIENT_CPP_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/include/rosbridge_client_cpp/vendor
)
set(ROSBRIDGE_CLIENT_CPP_EXT_LIBS
    pthread ssl crypto z curl
)
set(ROSBRIDGE_CLIENT_CPP_LIBS
    rosbridge_client_cpp ${ROSBRIDGE_CLIENT_CPP_EXT_LIBS}
)
set(ROSBRIDGE_CLIENT_GTEST
    ${CMAKE_CURRENT_LIST_DIR}/googletest-1.8.0/src/gtest-all.cc
)

#### LIBRARIES #########################################################################################################

file(GLOB_RECURSE ROSBRIDGE_CLIENT_CPP_SRC_GLOB
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)
include_directories(
    ${ROSBRIDGE_CLIENT_CPP_INCLUDES}
)
add_library(rosbridge_client_cpp SHARED
    ${ROSBRIDGE_CLIENT_CPP_SRC_GLOB}
)
target_link_libraries(rosbridge_client_cpp
    ${ROSBRIDGE_CLIENT_CPP_EXT_LIBS}
)

#### EXECUTABLES #######################################################################################################

## rosbridge_client_cpp_examples
##
add_executable(rosbridge_client_cpp_examples
    ${CMAKE_CURRENT_LIST_DIR}/tests/examples.cpp
)
target_link_libraries(rosbridge_client_cpp_examples
    ${ROSBRIDGE_CLIENT_CPP_LIBS}
)

## rosbridge_client_cpp_unit_tests
##
file(GLOB_RECURSE ROSBRIDGE_CLIENT_CPP_TESTS_GLOB
    ${CMAKE_CURRENT_LIST_DIR}/tests/unit_test_*.cpp
)
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/googletest-1.8.0
    ${CMAKE_CURRENT_LIST_DIR}/googletest-1.8.0/include
)
add_executable(rosbridge_client_cpp_unit_tests
    ${ROSBRIDGE_CLIENT_CPP_TESTS_GLOB}
    ${ROSBRIDGE_CLIENT_GTEST}
    ${CMAKE_CURRENT_LIST_DIR}/tests/unit_tests.cpp
)
target_link_libraries(rosbridge_client_cpp_unit_tests
    ${ROSBRIDGE_CLIENT_CPP_LIBS}
)

#### INSTALL ###########################################################################################################

install(TARGETS
    rosbridge_client_cpp
    DESTINATION lib
)
install(DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/include/rosbridge_client_cpp
    DESTINATION include
)

#### OTHER #############################################################################################################

## dummy target for including all files in the project
##
file(GLOB_RECURSE ROSBRIDGE_CLIENT_CPP_GLOB
    ${CMAKE_CURRENT_LIST_DIR}/include/*.h
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)
add_custom_target(rosbridge_client_cpp_files SOURCES ${ROSBRIDGE_CLIENT_CPP_GLOB})
