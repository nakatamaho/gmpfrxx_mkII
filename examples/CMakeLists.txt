add_executable(example01 example01.cpp)
target_link_libraries(example01 PRIVATE gmpxx_mkII)

add_executable(example02_mpfr_mpc example02_mpfr_mpc.cpp)
target_link_libraries(example02_mpfr_mpc PRIVATE mpfrxx_mkII)

add_executable(example03_mpfc_math example03_mpfc_math.cpp)
target_link_libraries(example03_mpfc_math PRIVATE gmpxx_mkII)

add_test(NAME example01 COMMAND example01)
add_test(NAME example02_mpfr_mpc COMMAND example02_mpfr_mpc)
add_test(NAME example03_mpfc_math COMMAND example03_mpfc_math)
