# Copyright (C) Codeplay Software Limited
#
# Licensed under the Apache License, Version 2.0 (the "License") with LLVM
# Exceptions; you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://github.com/codeplaysoftware/oneapi-construction-kit/blob/main/LICENSE.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.20)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(hal_refsi VERSION 1.0.0)

  if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-tree builds are not supported")
  endif()

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
  string(APPEND CMAKE_INSTALL_RPATH :$ORIGIN/../lib)
endif()

set(NUM_HARTS_FOR_CA_MODE "2" CACHE STRING "Number of used harts for the simulator.")

set(HAL_REFSI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH ${HAL_REFSI_SOURCE_DIR}/cmake)

include(CompileKernel)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
string(APPEND CMAKE_C_FLAGS " -fvisibility=hidden")
string(APPEND CMAKE_CXX_FLAGS " -fvisibility=hidden")

set_property(GLOBAL APPEND PROPERTY KNOWN_HAL_DEVICES "refsi")
set_property(GLOBAL PROPERTY HAL_REFSI_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(HAL_REFSI_SOC "G1" CACHE STRING "Name of the RefSi SoC family to target")
set_property(CACHE HAL_REFSI_SOC PROPERTY STRINGS "M1;G1")

# Determine the kernel interface to use when compiling clik examples.
set(HAL_REFSI_THREAD_MODE "WG" CACHE STRING "Threading mode to use for the RefSi HAL. Valid values are 'WG' (work-group-per-thread) and 'WI' (work-item-per-thread)")
set_property(CACHE HAL_REFSI_THREAD_MODE PROPERTY STRINGS "WG;WI")
if (HAL_REFSI_SOC STREQUAL "M1" OR HAL_REFSI_THREAD_MODE STREQUAL "WG")
  set(EXAMPLES_IF "wg")
else()
  set(EXAMPLES_IF "wi")
endif()
set_property(GLOBAL PROPERTY HAL_REFSI_EXAMPLE_DIR "${HAL_REFSI_SOURCE_DIR}/examples/${EXAMPLES_IF}")

add_subdirectory(external)

include(Bin2H)
include(BakedFile)
include(FindCompiler)

# Report a HAL description and device name based on which SoC family has been
# selected.
if (HAL_REFSI_SOC STREQUAL G1)
  get_property(REFSIDRV_G1_ISA GLOBAL PROPERTY REFSIDRV_G1_ISA)
  set_property(GLOBAL PROPERTY HAL_REFSI_DESCRIPTION ${REFSIDRV_G1_ISA})
  if(REFSIDRV_G1_ISA MATCHES "^[rR][vV]32.*")
    set_property(GLOBAL PROPERTY HAL_REFSI_DEVICE_NAME "RefSi G1 RV32")
    message(STATUS "Targeting: RV32")
    set(TRIPLE "riscv32")
  else()
    set_property(GLOBAL PROPERTY HAL_REFSI_DEVICE_NAME "RefSi G1 RV64")
    message(STATUS "Targeting: RV64")
    set(TRIPLE "riscv64")
  endif()
else()
  get_property(REFSIDRV_M1_ISA GLOBAL PROPERTY REFSIDRV_M1_ISA)
  set_property(GLOBAL PROPERTY HAL_REFSI_DESCRIPTION ${REFSIDRV_M1_ISA})
  set_property(GLOBAL PROPERTY HAL_REFSI_DEVICE_NAME "RefSi M1")
  set(TRIPLE "riscv64")
endif()

find_cc(${TRIPLE})

if(NOT EXISTS "${RISCV_CC}")
  message(FATAL_ERROR "Could not find any RISC-V toolchain. Are RISCV_TOOLCHAIN_DIR and/or RISCV_GNU_TOOLCHAIN_DIR configured for an existing toolchain?")
endif()

add_subdirectory(source)
