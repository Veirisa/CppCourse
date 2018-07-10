cmake_minimum_required(VERSION 3.9)
project(persistent_set)

set(CMAKE_CXX_STANDARD 11)

add_executable(persistent_set main.cpp persistent_set.cpp persistent_set.h)