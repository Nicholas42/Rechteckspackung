cmake_minimum_required (VERSION 2.6)
project (Rechteckspackung)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_BUILD_TYPE Release)
include(Warnings.cmake)

add_custom_target(common.h)
add_executable(rechteckspackung.out main.cpp packing.cpp rectangle.cpp net.cpp bitmap.cpp min_cost_flow.cpp sequence_pair.cpp placement_iterator.cpp input_parser.cpp)
