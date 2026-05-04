add_executable(example01 example01.cpp)
target_link_libraries(example01 PRIVATE gmpxx_mkII)

add_test(NAME example01 COMMAND example01)
