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

if(NOT DEFINED CXX_COMPILER)
    message(FATAL_ERROR "CXX_COMPILER is required")
endif()
if(NOT DEFINED SOURCE)
    message(FATAL_ERROR "SOURCE is required")
endif()
if(NOT DEFINED OUTPUT)
    message(FATAL_ERROR "OUTPUT is required")
endif()
if(NOT DEFINED EXPECTED_DIAGNOSTIC)
    message(FATAL_ERROR "EXPECTED_DIAGNOSTIC is required")
endif()

set(command "${CXX_COMPILER}" -std=c++17)
foreach(flag IN LISTS CXX_FLAGS)
    list(APPEND command "${flag}")
endforeach()
foreach(include_dir IN LISTS INCLUDE_DIRS)
    list(APPEND command "-I${include_dir}")
endforeach()
list(APPEND command -c "${SOURCE}" -o "${OUTPUT}")

execute_process(
    COMMAND ${command}
    RESULT_VARIABLE compile_result
    OUTPUT_VARIABLE compile_stdout
    ERROR_VARIABLE compile_stderr)

set(compile_output "${compile_stdout}\n${compile_stderr}")

if(compile_result EQUAL 0)
    message(FATAL_ERROR "compile-fail test unexpectedly compiled: ${SOURCE}")
endif()

if(NOT compile_output MATCHES "${EXPECTED_DIAGNOSTIC}")
    message(FATAL_ERROR
        "compile-fail test failed for the wrong reason: ${SOURCE}\n"
        "Expected diagnostic regex: ${EXPECTED_DIAGNOSTIC}\n"
        "Compiler output:\n${compile_output}")
endif()

message(STATUS "compile-fail test rejected ${SOURCE} with the expected diagnostic")
