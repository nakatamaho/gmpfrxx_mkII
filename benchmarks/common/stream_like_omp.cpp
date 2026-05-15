/*
 * Copyright (c) 2026
 *      Nakata, Maho
 *      All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <vector>

namespace {

using clock_type = std::chrono::steady_clock;

double seconds_since(clock_type::time_point start, clock_type::time_point end) {
    return std::chrono::duration<double>(end - start).count();
}

void *checked_aligned_alloc(std::size_t alignment, std::size_t bytes) {
    void *ptr = nullptr;
    if (posix_memalign(&ptr, alignment, bytes) != 0 || ptr == nullptr) {
        std::cerr << "posix_memalign failed for " << bytes << " bytes\n";
        std::exit(EXIT_FAILURE);
    }
    return ptr;
}

} // namespace

int main(int argc, char **argv) {
    const std::size_t n = argc > 1 ? std::strtoull(argv[1], nullptr, 10) : 100000000ULL;
    const int repeats = argc > 2 ? std::atoi(argv[2]) : 20;
    constexpr double scalar = 3.0;

    const std::size_t bytes_per_array = n * sizeof(double);
    double *a = static_cast<double *>(checked_aligned_alloc(64, bytes_per_array));
    double *b = static_cast<double *>(checked_aligned_alloc(64, bytes_per_array));
    double *c = static_cast<double *>(checked_aligned_alloc(64, bytes_per_array));

    std::cout << "STREAM_LIKE n=" << n << " repeats=" << repeats
              << " bytes_per_array=" << bytes_per_array
              << " total_arrays_bytes=" << (3.0 * static_cast<double>(bytes_per_array))
              << " threads=" << omp_get_max_threads() << "\n";

    const char *places = std::getenv("OMP_PLACES");
    const char *bind = std::getenv("OMP_PROC_BIND");
    const char *threads = std::getenv("OMP_NUM_THREADS");
    std::cout << "OPENMP OMP_NUM_THREADS=" << (threads ? threads : "")
              << " OMP_PLACES=" << (places ? places : "")
              << " OMP_PROC_BIND=" << (bind ? bind : "") << "\n";

#pragma omp parallel
    {
#pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
            a[i] = 1.0;
            b[i] = 2.0;
            c[i] = 0.0;
        }
    }

    struct Result {
        const char *name;
        double seconds;
        double gb_per_second;
    };

    std::vector<Result> results;

    auto run_copy = [&] {
        clock_type::time_point start;
        clock_type::time_point end;
#pragma omp parallel
        {
#pragma omp barrier
#pragma omp single
            start = clock_type::now();
            for (int r = 0; r < repeats; ++r) {
#pragma omp for schedule(static)
                for (std::size_t i = 0; i < n; ++i) {
                    c[i] = a[i];
                }
            }
#pragma omp barrier
#pragma omp single
            end = clock_type::now();
        }
        return seconds_since(start, end);
    };

    auto run_scale = [&] {
        clock_type::time_point start;
        clock_type::time_point end;
#pragma omp parallel
        {
#pragma omp barrier
#pragma omp single
            start = clock_type::now();
            for (int r = 0; r < repeats; ++r) {
#pragma omp for schedule(static)
                for (std::size_t i = 0; i < n; ++i) {
                    b[i] = scalar * c[i];
                }
            }
#pragma omp barrier
#pragma omp single
            end = clock_type::now();
        }
        return seconds_since(start, end);
    };

    auto run_add = [&] {
        clock_type::time_point start;
        clock_type::time_point end;
#pragma omp parallel
        {
#pragma omp barrier
#pragma omp single
            start = clock_type::now();
            for (int r = 0; r < repeats; ++r) {
#pragma omp for schedule(static)
                for (std::size_t i = 0; i < n; ++i) {
                    c[i] = a[i] + b[i];
                }
            }
#pragma omp barrier
#pragma omp single
            end = clock_type::now();
        }
        return seconds_since(start, end);
    };

    auto run_triad = [&] {
        clock_type::time_point start;
        clock_type::time_point end;
#pragma omp parallel
        {
#pragma omp barrier
#pragma omp single
            start = clock_type::now();
            for (int r = 0; r < repeats; ++r) {
#pragma omp for schedule(static)
                for (std::size_t i = 0; i < n; ++i) {
                    a[i] = b[i] + scalar * c[i];
                }
            }
#pragma omp barrier
#pragma omp single
            end = clock_type::now();
        }
        return seconds_since(start, end);
    };

    const double copy_seconds = run_copy();
    results.push_back({"Copy", copy_seconds, (2.0 * bytes_per_array * repeats) / copy_seconds / 1.0e9});
    const double scale_seconds = run_scale();
    results.push_back({"Scale", scale_seconds, (2.0 * bytes_per_array * repeats) / scale_seconds / 1.0e9});
    const double add_seconds = run_add();
    results.push_back({"Add", add_seconds, (3.0 * bytes_per_array * repeats) / add_seconds / 1.0e9});
    const double triad_seconds = run_triad();
    results.push_back({"Triad", triad_seconds, (3.0 * bytes_per_array * repeats) / triad_seconds / 1.0e9});

    double checksum = 0.0;
#pragma omp parallel for schedule(static) reduction(+ : checksum)
    for (std::size_t i = 0; i < n; ++i) {
        checksum += a[i] + b[i] + c[i];
    }

    std::cout << std::fixed << std::setprecision(3);
    for (const auto &result : results) {
        std::cout << result.name << " seconds=" << result.seconds
                  << " GB_per_s=" << result.gb_per_second << "\n";
    }
    std::cout << std::scientific << std::setprecision(6) << "CHECKSUM " << checksum << "\n";

    std::free(a);
    std::free(b);
    std::free(c);
    return 0;
}
