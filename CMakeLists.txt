cmake_minimum_required(VERSION 3.6)
project(SDTable)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
        SDTable.cpp
        SDTable.h
        )

add_executable(SDTable ${SOURCE_FILES})