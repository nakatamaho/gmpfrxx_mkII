add_executable(example01 example01.cpp)
target_link_libraries(example01 PRIVATE gmpxx_mkII)

add_executable(example02_mpf example02_mpf.cpp)
target_link_libraries(example02_mpf PRIVATE gmpxx_mkII)

add_executable(example02_mpfr example02_mpfr.cpp)
target_link_libraries(example02_mpfr PRIVATE mpfrxx_mkII)

add_executable(example02_mpfr_mpc example02_mpfr_mpc.cpp)
target_link_libraries(example02_mpfr_mpc PRIVATE mpfrxx_mkII)

add_executable(example03_mpf example03_mpf.cpp)
target_link_libraries(example03_mpf PRIVATE gmpxx_mkII)

add_executable(example03_mpfr example03_mpfr.cpp)
target_link_libraries(example03_mpfr PRIVATE mpfrxx_mkII)

add_executable(example03_mpfc_math example03_mpfc_math.cpp)
target_link_libraries(example03_mpfc_math PRIVATE gmpxx_mkII)

add_executable(example04_mpf example04_mpf.cpp)
target_link_libraries(example04_mpf PRIVATE gmpxx_mkII)

add_executable(example04_mpfr example04_mpfr.cpp)
target_link_libraries(example04_mpfr PRIVATE mpfrxx_mkII)

add_executable(example05_mpf example05_mpf.cpp)
target_link_libraries(example05_mpf PRIVATE gmpxx_mkII)

add_executable(example05_mpfr example05_mpfr.cpp)
target_link_libraries(example05_mpfr PRIVATE mpfrxx_mkII)

add_executable(example06_mpfc example06_mpfc.cpp)
target_link_libraries(example06_mpfc PRIVATE gmpxx_mkII)

add_executable(example06_mpc example06_mpc.cpp)
target_link_libraries(example06_mpc PRIVATE mpfrxx_mkII)

add_executable(example07_mpfc example07_mpfc.cpp)
target_link_libraries(example07_mpfc PRIVATE gmpxx_mkII)

add_executable(example07_mpc example07_mpc.cpp)
target_link_libraries(example07_mpc PRIVATE mpfrxx_mkII)

add_test(NAME example01 COMMAND example01)
add_test(NAME example02_mpf COMMAND example02_mpf)
add_test(NAME example02_mpfr COMMAND example02_mpfr)
add_test(NAME example02_mpfr_mpc COMMAND example02_mpfr_mpc)
add_test(NAME example03_mpf COMMAND example03_mpf)
add_test(NAME example03_mpfr COMMAND example03_mpfr)
add_test(NAME example03_mpfc_math COMMAND example03_mpfc_math)
add_test(NAME example04_mpf COMMAND example04_mpf)
add_test(NAME example04_mpfr COMMAND example04_mpfr)
add_test(NAME example05_mpf COMMAND example05_mpf)
add_test(NAME example05_mpfr COMMAND example05_mpfr)
add_test(NAME example06_mpfc COMMAND example06_mpfc)
add_test(NAME example06_mpc COMMAND example06_mpc)
add_test(NAME example07_mpfc COMMAND example07_mpfc --width 24 --height 10 --iterations 24)
add_test(NAME example07_mpc COMMAND example07_mpc --width 24 --height 10 --iterations 24)
