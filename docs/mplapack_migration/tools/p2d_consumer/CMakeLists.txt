cmake_minimum_required(VERSION 3.16)
project(gmpfrxx_mkII_p2d_consumer LANGUAGES CXX)

find_package(gmpfrxx_mkII 1.1.0 EXACT CONFIG REQUIRED)

set(P2D_QD_INCLUDE_DIR "" CACHE PATH "Prefix containing qd/dd_real.h")
if(NOT EXISTS "${P2D_QD_INCLUDE_DIR}/qd/dd_real.h")
    message(FATAL_ERROR "P2D_QD_INCLUDE_DIR must contain qd/dd_real.h")
endif()

add_executable(p2d_consumer main.cpp)
target_compile_features(p2d_consumer PRIVATE cxx_std_17)
target_include_directories(p2d_consumer SYSTEM PRIVATE "${P2D_QD_INCLUDE_DIR}")
target_link_libraries(p2d_consumer PRIVATE gmpfrxx_mkII::gmpfrxx_mkII)
