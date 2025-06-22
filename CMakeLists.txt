cmake_minimum_required(VERSION 3.31)
project(DATA_STRUCTURES_WET2)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(DATA_STRUCTURES_WET2
        dspotify25b2.cpp
        dspotify25b2.h
        main25b2.cpp
        wet2util.h)
