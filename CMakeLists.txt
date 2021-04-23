cmake_minimum_required(VERSION 3.0)

project(pro)

SET(CMAKE_CXX_FLAGS -pthread)
find_package(RapidJSON REQUIRED)

include_directories(${RapidJSON_INCLUDE_DIR})
include_directories(include)

link_directories("lib")

add_executable(testC test.cpp)


target_link_libraries(testC ${RapidJSON_LIBS})
target_link_libraries(testC libmuduo_net.a libmuduo_base.a)
