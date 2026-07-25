cmake_minimum_required(VERSION 3.16)
project(gmpfrxx_mkII_p2b_consumer LANGUAGES CXX)

find_package(gmpfrxx_mkII CONFIG REQUIRED)

add_executable(p2b_consumer main.cpp)
target_compile_features(p2b_consumer PRIVATE cxx_std_17)
target_link_libraries(p2b_consumer PRIVATE gmpfrxx_mkII::gmpfrxx_mkII)
