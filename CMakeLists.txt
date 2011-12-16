cmake_minimum_required(VERSION 2.8)
project(algos)

set(CMAKE_BUILD_TYPE "Debug")

# add_subdirectory(queue)
include_directories(queue)

add_executable(alg main.cpp
    queue/queue.cpp
    maxgap.cpp
    disjointset.cpp
    max_sub_set.cpp
    tinylog.h
    stdlog.cpp
)
