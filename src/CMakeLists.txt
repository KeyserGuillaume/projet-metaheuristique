cmake_minimum_required(VERSION 2.6)

project(ProjetRO)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++11")

add_executable(ProjetRO main.cpp target.h target.cpp types.h field.cpp field.h local_search.cpp local_search.h cost_functions.cpp cost_functions.h)

