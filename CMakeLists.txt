cmake_minimum_required (VERSION 2.6)
project (Rechteckspackung)

set (CMAKE_CXX_STANDARD 11)

add_executable(Rechteckspackung main.cpp packing.cpp rectangle.cpp net.cpp)