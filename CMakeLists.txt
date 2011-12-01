cmake_minimum_required(VERSION 2.8)
project(algos)

# add_subdirectory(queue)
include_directories(queue)

add_executable(alg main.cpp
    queue/queue.cpp
    tinylog.h
)
