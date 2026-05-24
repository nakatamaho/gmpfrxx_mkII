# Copyright (c) 2026
#      Nakata, Maho
#      All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

find_package(OpenMP)
find_library(BENCHMARK_GMPXX_LIBRARY NAMES gmpxx)

option(GMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS
    "Count mpf_init, mpf_clear, and selected mpf arithmetic calls in instrumented GMP benchmarks"
    OFF)
option(GMPFRXX_MKII_BENCHMARK_COUNT_MPFR_OPERATIONS
    "Count mpfr_init, mpfr_init2, mpfr_clear, mpfr_add, and mpfr_mul calls in instrumented MPFR benchmarks"
    OFF)

if(NOT BENCHMARK_GMPXX_LIBRARY)
    message(WARNING "libgmpxx was not found; upstream gmpxx.h orig benchmark variants will not be built")
endif()

function(configure_eager_benchmark target subdir)
    target_include_directories(${target} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/common"
        "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${subdir}")
    if(target MATCHES "openmp" AND OpenMP_CXX_FOUND)
        target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)
    endif()
    if(GMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS)
        target_compile_definitions(${target} PRIVATE
            GMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS)
    endif()
    if(GMPFRXX_MKII_BENCHMARK_COUNT_MPFR_OPERATIONS)
        target_compile_definitions(${target} PRIVATE
            GMPFRXX_MKII_BENCHMARK_COUNT_MPFR_OPERATIONS)
    endif()
endfunction()

function(add_mkii_variant subdir source base suffix)
    set(target "${base}_${suffix}")
    add_executable(${target} "${subdir}/${source}")
    configure_eager_benchmark(${target} ${subdir})
    target_link_libraries(${target} PRIVATE gmpxx_mkII::gmpxx_mkII)
    if(suffix STREQUAL "mkII_FIXED_PRECISION_FASTPATH")
        target_compile_definitions(${target} PRIVATE
            GMPFRXX_MKII_FAST_FIXED_PREC)
    endif()
endfunction()

function(add_gmpxx_variant subdir source base)
    if(NOT BENCHMARK_GMPXX_LIBRARY)
        return()
    endif()
    set(target "${base}_orig")
    add_executable(${target} "${subdir}/${source}")
    configure_eager_benchmark(${target} ${subdir})
    target_compile_definitions(${target} PRIVATE USE_ORIGINAL_GMPXX)
    target_link_libraries(${target} PRIVATE "${BENCHMARK_GMPXX_LIBRARY}" GMP::GMP)
endfunction()

function(add_kernel_variants subdir source base)
    add_gmpxx_variant(${subdir} ${source} ${base})
    add_mkii_variant(${subdir} ${source} ${base} mkII)
    add_mkii_variant(${subdir} ${source} ${base} mkII_FIXED_PRECISION_FASTPATH)
endfunction()

function(add_rdot_kernel_variants subdir source base)
    add_gmpxx_variant(${subdir} ${source} ${base})
    add_mkii_variant(${subdir} ${source} ${base} mkII)
    add_mkii_variant(${subdir} ${source} ${base} mkII_FIXED_PRECISION_FASTPATH)
endfunction()

function(add_native_benchmark subdir source target)
    add_executable(${target} "${subdir}/${source}")
    configure_eager_benchmark(${target} ${subdir})
    target_link_libraries(${target} PRIVATE gmpxx_mkII::gmpxx_mkII GMP::GMP)
endfunction()

function(add_rdot_native_microbench source target)
    add_executable(${target} "gmp/00_Rdot/${source}")
    configure_eager_benchmark(${target} gmp/00_Rdot)
    target_link_libraries(${target} PRIVATE GMP::GMP)
endfunction()

function(add_mpfr_mkii_variant subdir source base suffix)
    set(target "${base}_${suffix}")
    add_executable(${target} "${subdir}/${source}")
    configure_eager_benchmark(${target} ${subdir})
    target_link_libraries(${target} PRIVATE mpfrxx_mkII::mpfrxx_mkII)
    if(suffix MATCHES "STABLE_ROUNDING")
        target_compile_definitions(${target} PRIVATE
            GMPFRXX_MKII_FAST_STABLE_RND)
    endif()
    if(suffix MATCHES "FIXED_PRECISION_FASTPATH")
        target_compile_definitions(${target} PRIVATE
            GMPFRXX_MKII_FAST_FIXED_PREC)
    endif()
    if(suffix MATCHES "FMA")
        target_compile_definitions(${target} PRIVATE
            GMPFRXX_MKII_ENABLE_FMA)
    endif()
endfunction()

function(add_mpfr_kernel_variants subdir source base)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII_FIXED_PRECISION_FASTPATH)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII_FIXED_PRECISION_FASTPATH_FMA)
endfunction()

function(add_mpfr_rdot_kernel_variants subdir source base)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII_STABLE_ROUNDING)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII_STABLE_ROUNDING_FMA)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH)
endfunction()

function(add_mpfr_rdot_cpp_target subdir source target)
    add_executable(${target} "${subdir}/${source}")
    configure_eager_benchmark(${target} ${subdir})
    target_link_libraries(${target} PRIVATE mpfrxx_mkII::mpfrxx_mkII)
    if(target MATCHES "PRECISION")
        target_compile_definitions(${target} PRIVATE
            GMPFRXX_MKII_FAST_FIXED_PREC)
    endif()
    if(target MATCHES "_FMA$")
        target_compile_definitions(${target} PRIVATE
            GMPFRXX_MKII_ENABLE_FMA)
    endif()
endfunction()

function(add_mpfr_rdot_base_variants subdir source base)
    add_mpfr_rdot_cpp_target(${subdir} ${source} ${base})
    add_mpfr_rdot_cpp_target(${subdir} ${source} ${base}_PRECISION)
endfunction()

function(add_mpfr_rdot_rounding_variants subdir source base)
    add_mpfr_rdot_cpp_target(${subdir} ${source} ${base}_ROUNDING)
    add_mpfr_rdot_cpp_target(${subdir} ${source} ${base}_ROUNDING_PRECISION)
endfunction()

function(add_mpfr_rdot_fma_capture_variants subdir source base)
    add_mpfr_rdot_cpp_target(${subdir} ${source} ${base}_ROUNDING_FMA_CAPTURE_FMA)
    add_mpfr_rdot_cpp_target(${subdir} ${source} ${base}_ROUNDING_FMA_CAPTURE_PRECISION_FMA)
endfunction()

function(add_mpfr_raxpy_kernel_variants subdir source base)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII_STABLE_ROUNDING)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII_STABLE_ROUNDING_FMA)
    add_mpfr_mkii_variant(${subdir} ${source} ${base} mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH)
endfunction()

function(add_mpfr_rgemv_cpp_target subdir source target)
    add_executable(${target} "${subdir}/${source}")
    configure_eager_benchmark(${target} ${subdir})
    target_link_libraries(${target} PRIVATE mpfrxx_mkII::mpfrxx_mkII)
    if(target MATCHES "PRECISION")
        target_compile_definitions(${target} PRIVATE
            GMPFRXX_MKII_FAST_FIXED_PREC)
    endif()
    if(target MATCHES "_FMA$")
        target_compile_definitions(${target} PRIVATE
            GMPFRXX_MKII_ENABLE_FMA)
    endif()
endfunction()

function(add_mpfr_rgemv_base_variants subdir source base)
    add_mpfr_rgemv_cpp_target(${subdir} ${source} ${base})
    add_mpfr_rgemv_cpp_target(${subdir} ${source} ${base}_PRECISION)
endfunction()

function(add_mpfr_rgemv_rounding_variants subdir source base)
    add_mpfr_rgemv_cpp_target(${subdir} ${source} ${base}_ROUNDING)
    add_mpfr_rgemv_cpp_target(${subdir} ${source} ${base}_ROUNDING_PRECISION)
endfunction()

function(add_mpfr_rgemv_fma_capture_variants subdir source base)
    add_mpfr_rgemv_cpp_target(${subdir} ${source} ${base}_ROUNDING_FMA_CAPTURE_FMA)
    add_mpfr_rgemv_cpp_target(${subdir} ${source} ${base}_ROUNDING_FMA_CAPTURE_PRECISION_FMA)
endfunction()

function(add_mpfr_rgemv_variant_family subdir base)
    add_mpfr_rgemv_base_variants(${subdir} ${base}.cpp ${base})
    add_mpfr_rgemv_rounding_variants(${subdir} ${base}_ROUNDING.cpp ${base})
    add_mpfr_rgemv_fma_capture_variants(${subdir} ${base}_ROUNDING_FMA_CAPTURE.cpp ${base})
endfunction()

function(add_mpfr_native_benchmark subdir source target)
    add_executable(${target} "${subdir}/${source}")
    configure_eager_benchmark(${target} ${subdir})
    target_link_libraries(${target} PRIVATE mpfrxx_mkII::mpfrxx_mkII)
endfunction()

add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_01.cpp
    Rdot_gmp_C_native_01)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_02.cpp
    Rdot_gmp_C_native_02)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_03.cpp
    Rdot_gmp_C_native_03)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_04.cpp
    Rdot_gmp_C_native_04)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_05.cpp
    Rdot_gmp_C_native_05)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_06.cpp
    Rdot_gmp_C_native_06)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_openmp_01.cpp
    Rdot_gmp_C_native_openmp_01)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_openmp_02.cpp
    Rdot_gmp_C_native_openmp_02)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_openmp_03.cpp
    Rdot_gmp_C_native_openmp_03)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_openmp_04.cpp
    Rdot_gmp_C_native_openmp_04)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_openmp_05.cpp
    Rdot_gmp_C_native_openmp_05)
add_native_benchmark(gmp/00_Rdot Rdot_gmp_C_native_openmp_06.cpp
    Rdot_gmp_C_native_openmp_06)
add_rdot_native_microbench(Rdot_gmp_C_native_07_readonly.cpp Rdot_gmp_C_native_07_readonly)
add_rdot_native_microbench(Rdot_gmp_C_native_08_addonly.cpp Rdot_gmp_C_native_08_addonly)
add_rdot_native_microbench(Rdot_gmp_C_native_09_mulonly.cpp Rdot_gmp_C_native_09_mulonly)
add_rdot_native_microbench(Rdot_gmp_C_native_10_muladd.cpp Rdot_gmp_C_native_10_muladd)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_01.cpp Rdot_gmp_kernel_01)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_02.cpp Rdot_gmp_kernel_02)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_03.cpp Rdot_gmp_kernel_03)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_04.cpp Rdot_gmp_kernel_04)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_05.cpp Rdot_gmp_kernel_05)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_06.cpp Rdot_gmp_kernel_06)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_openmp_01.cpp
    Rdot_gmp_kernel_openmp_01)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_openmp_02.cpp
    Rdot_gmp_kernel_openmp_02)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_openmp_03.cpp
    Rdot_gmp_kernel_openmp_03)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_openmp_04.cpp
    Rdot_gmp_kernel_openmp_04)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_openmp_05.cpp
    Rdot_gmp_kernel_openmp_05)
add_rdot_kernel_variants(gmp/00_Rdot Rdot_gmp_kernel_openmp_06.cpp
    Rdot_gmp_kernel_openmp_06)

add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_01.cpp
    Rdot_mpfr_C_native_01)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_02.cpp
    Rdot_mpfr_C_native_02)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_03.cpp
    Rdot_mpfr_C_native_03)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_04.cpp
    Rdot_mpfr_C_native_04)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_05.cpp
    Rdot_mpfr_C_native_05)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_06.cpp
    Rdot_mpfr_C_native_06)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_01_FMA.cpp
    Rdot_mpfr_C_native_01_FMA)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_openmp_01.cpp
    Rdot_mpfr_C_native_openmp_01)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_openmp_02.cpp
    Rdot_mpfr_C_native_openmp_02)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_openmp_03.cpp
    Rdot_mpfr_C_native_openmp_03)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_openmp_04.cpp
    Rdot_mpfr_C_native_openmp_04)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_openmp_05.cpp
    Rdot_mpfr_C_native_openmp_05)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_openmp_06.cpp
    Rdot_mpfr_C_native_openmp_06)
add_mpfr_native_benchmark(mpfr/00_Rdot Rdot_mpfr_C_native_openmp_01_FMA.cpp
    Rdot_mpfr_C_native_openmp_01_FMA)
foreach(variant 01 02 03 04 05 06)
    add_mpfr_rdot_base_variants(mpfr/00_Rdot Rdot_mpfr_kernel_${variant}.cpp
        Rdot_mpfr_kernel_${variant})
endforeach()
add_mpfr_rdot_rounding_variants(mpfr/00_Rdot Rdot_mpfr_kernel_01_ROUNDING.cpp
    Rdot_mpfr_kernel_01)
add_mpfr_rdot_fma_capture_variants(mpfr/00_Rdot Rdot_mpfr_kernel_01_ROUNDING_FMA_CAPTURE.cpp
    Rdot_mpfr_kernel_01)
add_mpfr_rdot_rounding_variants(mpfr/00_Rdot Rdot_mpfr_kernel_03_ROUNDING.cpp
    Rdot_mpfr_kernel_03)
foreach(variant 01 02 03 04 05 06)
    add_mpfr_rdot_base_variants(mpfr/00_Rdot Rdot_mpfr_kernel_openmp_${variant}.cpp
        Rdot_mpfr_kernel_openmp_${variant})
endforeach()
add_mpfr_rdot_rounding_variants(mpfr/00_Rdot Rdot_mpfr_kernel_openmp_01_ROUNDING.cpp
    Rdot_mpfr_kernel_openmp_01)
add_mpfr_rdot_fma_capture_variants(mpfr/00_Rdot Rdot_mpfr_kernel_openmp_01_ROUNDING_FMA_CAPTURE.cpp
    Rdot_mpfr_kernel_openmp_01)
add_mpfr_rdot_rounding_variants(mpfr/00_Rdot Rdot_mpfr_kernel_openmp_03_ROUNDING.cpp
    Rdot_mpfr_kernel_openmp_03)

add_mpfr_native_benchmark(mpfr/01_Raxpy Raxpy_mpfr_C_native_01.cpp
    Raxpy_mpfr_C_native_01)
add_mpfr_native_benchmark(mpfr/01_Raxpy Raxpy_mpfr_C_native_01_FMA.cpp
    Raxpy_mpfr_C_native_01_FMA)
add_mpfr_native_benchmark(mpfr/01_Raxpy Raxpy_mpfr_C_native_packed_custom_layout_FMA.cpp
    Raxpy_mpfr_C_native_packed_custom_layout_FMA)
add_mpfr_native_benchmark(mpfr/01_Raxpy Raxpy_mpfr_C_native_openmp_01.cpp
    Raxpy_mpfr_C_native_openmp_01)
add_mpfr_native_benchmark(mpfr/01_Raxpy Raxpy_mpfr_C_native_openmp_01_FMA.cpp
    Raxpy_mpfr_C_native_openmp_01_FMA)
add_mpfr_raxpy_kernel_variants(mpfr/01_Raxpy Raxpy_mpfr_kernel_01.cpp
    Raxpy_mpfr_kernel_01)
add_mpfr_raxpy_kernel_variants(mpfr/01_Raxpy Raxpy_mpfr_kernel_02.cpp
    Raxpy_mpfr_kernel_02)
add_mpfr_raxpy_kernel_variants(mpfr/01_Raxpy Raxpy_mpfr_kernel_03.cpp
    Raxpy_mpfr_kernel_03)
add_mpfr_raxpy_kernel_variants(mpfr/01_Raxpy Raxpy_mpfr_kernel_04.cpp
    Raxpy_mpfr_kernel_04)
add_mpfr_mkii_variant(mpfr/01_Raxpy Raxpy_mpfr_kernel_05.cpp
    Raxpy_mpfr_kernel_05 mkII)
add_mpfr_mkii_variant(mpfr/01_Raxpy Raxpy_mpfr_kernel_05.cpp
    Raxpy_mpfr_kernel_05 mkII_FMA)
add_mpfr_mkii_variant(mpfr/01_Raxpy Raxpy_mpfr_kernel_06.cpp
    Raxpy_mpfr_kernel_06 mkII)
add_mpfr_raxpy_kernel_variants(mpfr/01_Raxpy Raxpy_mpfr_kernel_openmp_01.cpp
    Raxpy_mpfr_kernel_openmp_01)
add_mpfr_raxpy_kernel_variants(mpfr/01_Raxpy Raxpy_mpfr_kernel_openmp_02.cpp
    Raxpy_mpfr_kernel_openmp_02)
add_mpfr_raxpy_kernel_variants(mpfr/01_Raxpy Raxpy_mpfr_kernel_openmp_03.cpp
    Raxpy_mpfr_kernel_openmp_03)
add_mpfr_raxpy_kernel_variants(mpfr/01_Raxpy Raxpy_mpfr_kernel_openmp_04.cpp
    Raxpy_mpfr_kernel_openmp_04)
add_mpfr_mkii_variant(mpfr/01_Raxpy Raxpy_mpfr_kernel_openmp_05.cpp
    Raxpy_mpfr_kernel_openmp_05 mkII)
add_mpfr_mkii_variant(mpfr/01_Raxpy Raxpy_mpfr_kernel_openmp_05.cpp
    Raxpy_mpfr_kernel_openmp_05 mkII_FMA)
add_mpfr_mkii_variant(mpfr/01_Raxpy Raxpy_mpfr_kernel_openmp_06.cpp
    Raxpy_mpfr_kernel_openmp_06 mkII)

add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_01.cpp
    Rgemv_mpfr_C_native_01)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_01_FMA.cpp
    Rgemv_mpfr_C_native_01_FMA)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_02.cpp
    Rgemv_mpfr_C_native_02)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_02_FMA.cpp
    Rgemv_mpfr_C_native_02_FMA)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_03.cpp
    Rgemv_mpfr_C_native_03)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_04.cpp
    Rgemv_mpfr_C_native_04)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_01.cpp
    Rgemv_mpfr_C_native_openmp_01)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_01_FMA.cpp
    Rgemv_mpfr_C_native_openmp_01_FMA)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_02.cpp
    Rgemv_mpfr_C_native_openmp_02)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_02_FMA.cpp
    Rgemv_mpfr_C_native_openmp_02_FMA)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_03.cpp
    Rgemv_mpfr_C_native_openmp_03)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_04.cpp
    Rgemv_mpfr_C_native_openmp_04)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_04_FMA.cpp
    Rgemv_mpfr_C_native_openmp_04_FMA)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_05.cpp
    Rgemv_mpfr_C_native_openmp_05)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_05_FMA.cpp
    Rgemv_mpfr_C_native_openmp_05_FMA)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_06.cpp
    Rgemv_mpfr_C_native_openmp_06)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_06_FMA.cpp
    Rgemv_mpfr_C_native_openmp_06_FMA)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_07.cpp
    Rgemv_mpfr_C_native_openmp_07)
add_mpfr_native_benchmark(mpfr/02_Rgemv Rgemv_mpfr_C_native_openmp_07_FMA.cpp
    Rgemv_mpfr_C_native_openmp_07_FMA)
foreach(variant 01 02 03 04)
    add_mpfr_rgemv_variant_family(mpfr/02_Rgemv Rgemv_mpfr_kernel_${variant})
endforeach()
foreach(variant 01 02 03 04 05 06 07)
    add_mpfr_rgemv_variant_family(mpfr/02_Rgemv Rgemv_mpfr_kernel_openmp_${variant})
endforeach()

add_mpfr_native_benchmark(mpfr/03_Rgemm Rgemm_mpfr_C_native_01.cpp
    Rgemm_mpfr_C_native_01)
add_mpfr_native_benchmark(mpfr/03_Rgemm Rgemm_mpfr_C_native_02.cpp
    Rgemm_mpfr_C_native_02)
add_mpfr_native_benchmark(mpfr/03_Rgemm Rgemm_mpfr_C_native_openmp_01.cpp
    Rgemm_mpfr_C_native_openmp_01)
add_mpfr_native_benchmark(mpfr/03_Rgemm Rgemm_mpfr_C_native_openmp_02.cpp
    Rgemm_mpfr_C_native_openmp_02)
add_mpfr_kernel_variants(mpfr/03_Rgemm Rgemm_mpfr_kernel_01.cpp
    Rgemm_mpfr_kernel_01)
add_mpfr_kernel_variants(mpfr/03_Rgemm Rgemm_mpfr_kernel_02.cpp
    Rgemm_mpfr_kernel_02)
add_mpfr_kernel_variants(mpfr/03_Rgemm Rgemm_mpfr_kernel_03.cpp
    Rgemm_mpfr_kernel_03)
add_mpfr_kernel_variants(mpfr/03_Rgemm Rgemm_mpfr_kernel_openmp_01.cpp
    Rgemm_mpfr_kernel_openmp_01)
add_mpfr_kernel_variants(mpfr/03_Rgemm Rgemm_mpfr_kernel_openmp_02.cpp
    Rgemm_mpfr_kernel_openmp_02)
add_mpfr_kernel_variants(mpfr/03_Rgemm Rgemm_mpfr_kernel_openmp_03.cpp
    Rgemm_mpfr_kernel_openmp_03)

add_native_benchmark(gmp/01_Raxpy Raxpy_gmp_C_native_01.cpp
    Raxpy_gmp_C_native_01)
add_native_benchmark(gmp/01_Raxpy Raxpy_gmp_C_native_openmp_01.cpp
    Raxpy_gmp_C_native_openmp_01)
add_kernel_variants(gmp/01_Raxpy Raxpy_gmp_kernel_01.cpp Raxpy_gmp_kernel_01)
add_kernel_variants(gmp/01_Raxpy Raxpy_gmp_kernel_02.cpp Raxpy_gmp_kernel_02)
add_kernel_variants(gmp/01_Raxpy Raxpy_gmp_kernel_03.cpp Raxpy_gmp_kernel_03)
add_kernel_variants(gmp/01_Raxpy Raxpy_gmp_kernel_04.cpp Raxpy_gmp_kernel_04)
add_kernel_variants(gmp/01_Raxpy Raxpy_gmp_kernel_openmp_01.cpp
    Raxpy_gmp_kernel_openmp_01)
add_kernel_variants(gmp/01_Raxpy Raxpy_gmp_kernel_openmp_02.cpp
    Raxpy_gmp_kernel_openmp_02)
add_kernel_variants(gmp/01_Raxpy Raxpy_gmp_kernel_openmp_03.cpp
    Raxpy_gmp_kernel_openmp_03)

add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_01.cpp
    Rgemv_gmp_C_native_01)
add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_02.cpp
    Rgemv_gmp_C_native_02)
add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_03.cpp
    Rgemv_gmp_C_native_03)
add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_04.cpp
    Rgemv_gmp_C_native_04)
add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_openmp_01.cpp
    Rgemv_gmp_C_native_openmp_01)
add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_openmp_02.cpp
    Rgemv_gmp_C_native_openmp_02)
add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_openmp_03.cpp
    Rgemv_gmp_C_native_openmp_03)
add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_openmp_04.cpp
    Rgemv_gmp_C_native_openmp_04)
add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_openmp_05.cpp
    Rgemv_gmp_C_native_openmp_05)
add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_openmp_06.cpp
    Rgemv_gmp_C_native_openmp_06)
add_native_benchmark(gmp/02_Rgemv Rgemv_gmp_C_native_openmp_07.cpp
    Rgemv_gmp_C_native_openmp_07)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_01.cpp Rgemv_gmp_kernel_01)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_02.cpp Rgemv_gmp_kernel_02)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_03.cpp Rgemv_gmp_kernel_03)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_04.cpp Rgemv_gmp_kernel_04)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_openmp_01.cpp
    Rgemv_gmp_kernel_openmp_01)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_openmp_02.cpp
    Rgemv_gmp_kernel_openmp_02)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_openmp_03.cpp
    Rgemv_gmp_kernel_openmp_03)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_openmp_04.cpp
    Rgemv_gmp_kernel_openmp_04)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_openmp_05.cpp
    Rgemv_gmp_kernel_openmp_05)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_openmp_06.cpp
    Rgemv_gmp_kernel_openmp_06)
add_kernel_variants(gmp/02_Rgemv Rgemv_gmp_kernel_openmp_07.cpp
    Rgemv_gmp_kernel_openmp_07)

add_native_benchmark(gmp/03_Rgemm Rgemm_gmp_C_native_01.cpp
    Rgemm_gmp_C_native_01)
add_native_benchmark(gmp/03_Rgemm Rgemm_gmp_C_native_02.cpp
    Rgemm_gmp_C_native_02)
add_native_benchmark(gmp/03_Rgemm Rgemm_gmp_C_native_04.cpp
    Rgemm_gmp_C_native_04)
add_native_benchmark(gmp/03_Rgemm Rgemm_gmp_C_native_05.cpp
    Rgemm_gmp_C_native_05)
add_native_benchmark(gmp/03_Rgemm Rgemm_gmp_C_native_06.cpp
    Rgemm_gmp_C_native_06)
add_native_benchmark(gmp/03_Rgemm Rgemm_gmp_C_native_openmp_01.cpp
    Rgemm_gmp_C_native_openmp_01)
add_native_benchmark(gmp/03_Rgemm Rgemm_gmp_C_native_openmp_02.cpp
    Rgemm_gmp_C_native_openmp_02)
add_native_benchmark(gmp/03_Rgemm Rgemm_gmp_C_native_openmp_04.cpp
    Rgemm_gmp_C_native_openmp_04)
add_native_benchmark(gmp/03_Rgemm Rgemm_gmp_C_native_openmp_05.cpp
    Rgemm_gmp_C_native_openmp_05)
add_native_benchmark(gmp/03_Rgemm Rgemm_gmp_C_native_openmp_06.cpp
    Rgemm_gmp_C_native_openmp_06)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_01.cpp Rgemm_gmp_kernel_01)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_02.cpp Rgemm_gmp_kernel_02)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_03.cpp Rgemm_gmp_kernel_03)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_04.cpp Rgemm_gmp_kernel_04)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_05.cpp Rgemm_gmp_kernel_05)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_06.cpp Rgemm_gmp_kernel_06)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_openmp_01.cpp
                    Rgemm_gmp_kernel_openmp_01)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_openmp_02.cpp
                    Rgemm_gmp_kernel_openmp_02)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_openmp_03.cpp
                    Rgemm_gmp_kernel_openmp_03)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_openmp_04.cpp
                    Rgemm_gmp_kernel_openmp_04)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_openmp_05.cpp
                    Rgemm_gmp_kernel_openmp_05)
add_kernel_variants(gmp/03_Rgemm Rgemm_gmp_kernel_openmp_06.cpp
                    Rgemm_gmp_kernel_openmp_06)
