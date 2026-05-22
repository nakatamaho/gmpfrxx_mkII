add_executable(example01 example01.cpp)
target_link_libraries(example01 PRIVATE gmpxx_mkII)

add_executable(example02_mpf example02_mpf.cpp)
target_link_libraries(example02_mpf PRIVATE gmpxx_mkII)

add_executable(example02_mpfr example02_mpfr.cpp)
target_link_libraries(example02_mpfr PRIVATE mpfrxx_mkII)

add_executable(example02_mpc example02_mpc.cpp)
target_link_libraries(example02_mpc PRIVATE mpcxx_mkII)

add_executable(example03_mpf example03_mpf.cpp)
target_link_libraries(example03_mpf PRIVATE gmpxx_mkII)

add_executable(example03_mpfr example03_mpfr.cpp)
target_link_libraries(example03_mpfr PRIVATE mpfrxx_mkII)

add_executable(example03_mpfc example03_mpfc.cpp)
target_link_libraries(example03_mpfc PRIVATE gmpxx_mkII)

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
target_link_libraries(example06_mpc PRIVATE mpcxx_mkII)

add_executable(example07_mpfc example07_mpfc.cpp)
target_link_libraries(example07_mpfc PRIVATE gmpxx_mkII)

add_executable(example07_mpc example07_mpc.cpp)
target_link_libraries(example07_mpc PRIVATE mpcxx_mkII)

add_executable(example08_mpfc example08_mpfc.cpp)
target_link_libraries(example08_mpfc PRIVATE gmpxx_mkII)

add_executable(example08_mpc example08_mpc.cpp)
target_link_libraries(example08_mpc PRIVATE mpcxx_mkII)

add_executable(example09_mpfc example09_mpfc.cpp)
target_link_libraries(example09_mpfc PRIVATE gmpxx_mkII)

add_executable(example09_mpc example09_mpc.cpp)
target_link_libraries(example09_mpc PRIVATE mpcxx_mkII)

add_executable(example10_mpf example10_mpf.cpp)
target_link_libraries(example10_mpf PRIVATE gmpxx_mkII)

add_executable(example10_mpfr example10_mpfr.cpp)
target_link_libraries(example10_mpfr PRIVATE mpfrxx_mkII)

add_executable(example11_mpf example11_mpf.cpp)
target_link_libraries(example11_mpf PRIVATE gmpxx_mkII)

add_executable(example11_mpfr example11_mpfr.cpp)
target_link_libraries(example11_mpfr PRIVATE mpfrxx_mkII)

add_executable(example12_mpf example12_mpf.cpp)
target_link_libraries(example12_mpf PRIVATE gmpxx_mkII)

add_executable(example12_mpfr example12_mpfr.cpp)
target_link_libraries(example12_mpfr PRIVATE mpfrxx_mkII)

add_executable(example13_mpf example13_mpf.cpp)
target_link_libraries(example13_mpf PRIVATE gmpxx_mkII)

add_executable(example13_mpfr example13_mpfr.cpp)
target_link_libraries(example13_mpfr PRIVATE mpfrxx_mkII)

add_executable(example14_mpf example14_mpf.cpp)
target_link_libraries(example14_mpf PRIVATE gmpxx_mkII)

add_executable(example14_mpfr example14_mpfr.cpp)
target_link_libraries(example14_mpfr PRIVATE mpfrxx_mkII)

add_executable(example15_mpf example15_mpf.cpp)
target_link_libraries(example15_mpf PRIVATE gmpxx_mkII)

add_executable(example15_mpfr example15_mpfr.cpp)
target_link_libraries(example15_mpfr PRIVATE mpfrxx_mkII)

add_executable(example16_mpf_thread_context example16_mpf_thread_context.cpp)
target_link_libraries(example16_mpf_thread_context PRIVATE gmpxx_mkII_default_context_provider)

add_executable(example16_mpfr_thread_context example16_mpfr_thread_context.cpp)
target_link_libraries(example16_mpfr_thread_context PRIVATE mpfrxx_mkII)

add_test(NAME example01 COMMAND example01)
add_test(NAME example02_mpf COMMAND example02_mpf)
add_test(NAME example02_mpfr COMMAND example02_mpfr)
add_test(NAME example02_mpc COMMAND example02_mpc)
add_test(NAME example03_mpf COMMAND example03_mpf)
add_test(NAME example03_mpfr COMMAND example03_mpfr)
add_test(NAME example03_mpfc COMMAND example03_mpfc)
add_test(NAME example04_mpf COMMAND example04_mpf)
add_test(NAME example04_mpfr COMMAND example04_mpfr)
add_test(NAME example05_mpf COMMAND example05_mpf)
add_test(NAME example05_mpfr COMMAND example05_mpfr)
add_test(NAME example06_mpfc COMMAND example06_mpfc)
add_test(NAME example06_mpc COMMAND example06_mpc)
add_test(NAME example07_mpfc COMMAND example07_mpfc --width 24 --height 10 --iterations 24)
add_test(NAME example07_mpc COMMAND example07_mpc --width 24 --height 10 --iterations 24)
add_test(NAME example08_mpfc COMMAND example08_mpfc)
add_test(NAME example08_mpc COMMAND example08_mpc)
add_test(NAME example09_mpfc COMMAND example09_mpfc)
add_test(NAME example09_mpc COMMAND example09_mpc)
add_test(NAME example10_mpf COMMAND example10_mpf)
add_test(NAME example10_mpfr COMMAND example10_mpfr)
add_test(NAME example11_mpf COMMAND example11_mpf)
add_test(NAME example11_mpfr COMMAND example11_mpfr)
add_test(NAME example12_mpf COMMAND example12_mpf)
add_test(NAME example12_mpfr COMMAND example12_mpfr)
add_test(NAME example13_mpf COMMAND example13_mpf)
add_test(NAME example13_mpfr COMMAND example13_mpfr)
add_test(NAME example14_mpf COMMAND example14_mpf)
add_test(NAME example14_mpfr COMMAND example14_mpfr)
add_test(NAME example15_mpf COMMAND example15_mpf --digits 32)
add_test(NAME example15_mpfr COMMAND example15_mpfr --digits 32)
add_test(NAME example15_mpf_bbp COMMAND example15_mpf --constant pi --start 16 --digits 16 --method bbp)
add_test(NAME example15_mpfr_bbp COMMAND example15_mpfr --constant pi --start 16 --digits 16 --method bbp)
add_test(NAME example16_mpf_thread_context COMMAND example16_mpf_thread_context)
add_test(NAME example16_mpfr_thread_context COMMAND example16_mpfr_thread_context)
