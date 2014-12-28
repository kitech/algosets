cmake_minimum_required(VERSION 2.8)
project(algos)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_BUILD_TYPE "Debug")
add_definitions( -D_GNU_SOURCE ) # syscall need
set(CMAKE_CXX_FLAGS "-std=c++11")

# add_subdirectory(queue)
include_directories(queue)

add_executable(alg main.cpp
    queue/queue.cpp
    maxgap.cpp
    disjointset.cpp
    max_sub_set.cpp

    finddup.cpp

    tinylog.h
    stdlog.cpp
)


