// Copyright (C) Codeplay Software Limited
//
// Licensed under the Apache License, Version 2.0 (the "License") with LLVM
// Exceptions; you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://github.com/codeplaysoftware/oneapi-construction-kit/blob/main/LICENSE.txt
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>

#include "uur/fixtures.h"

struct urKernelSetArgMemObjTest : uur::ContextTest {
  void SetUp() override {
    UUR_RETURN_ON_FATAL_FAILURE(uur::ContextTest::SetUp());
    // Generated from the following OpenCL C:
    //
    // kernel void argument_test(global int* arg1, global int* arg2, global
    // int *arg3) {}
    const uint8_t source[] = {
        0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x06, 0x00,
        0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00,
        0x04, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x0b, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x4f, 0x70, 0x65, 0x6e,
        0x43, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00,
        0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x07, 0x00,
        0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x61, 0x72, 0x67, 0x75,
        0x6d, 0x65, 0x6e, 0x74, 0x5f, 0x74, 0x65, 0x73, 0x74, 0x00, 0x00, 0x00,
        0x07, 0x00, 0x0e, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x6b, 0x65, 0x72, 0x6e,
        0x65, 0x6c, 0x5f, 0x61, 0x72, 0x67, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x2e,
        0x61, 0x72, 0x67, 0x75, 0x6d, 0x65, 0x6e, 0x74, 0x5f, 0x74, 0x65, 0x73,
        0x74, 0x2e, 0x69, 0x6e, 0x74, 0x2a, 0x2c, 0x69, 0x6e, 0x74, 0x2a, 0x2c,
        0x69, 0x6e, 0x74, 0x2a, 0x2c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00,
        0x03, 0x00, 0x00, 0x00, 0xa0, 0x86, 0x01, 0x00, 0x05, 0x00, 0x04, 0x00,
        0x07, 0x00, 0x00, 0x00, 0x61, 0x72, 0x67, 0x31, 0x00, 0x00, 0x00, 0x00,
        0x05, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x61, 0x72, 0x67, 0x32,
        0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00,
        0x61, 0x72, 0x67, 0x33, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x00, 0x00, 0x00,
        0x47, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00,
        0x05, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x26, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00,
        0x09, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
        0x15, 0x00, 0x04, 0x00, 0x03, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x00, 0x00, 0x21, 0x00, 0x06, 0x00, 0x05, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00, 0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
        0x37, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
        0x37, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x37, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
        0xf8, 0x00, 0x02, 0x00, 0x0a, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00,
        0x38, 0x00, 0x01, 0x00};
    const size_t source_size = sizeof(source) / sizeof(source[0]);
    const char *kernel_name = "argument_test";

    ASSERT_SUCCESS(
        urProgramCreateWithIL(context, source, source_size, nullptr, &program));
    ASSERT_NE(nullptr, program);
    ASSERT_SUCCESS(urProgramBuild(context, program, nullptr));

    ASSERT_SUCCESS(urKernelCreate(program, kernel_name, &kernel));
    ASSERT_NE(nullptr, kernel);

    for (auto &buffer : buffers) {
      ASSERT_SUCCESS(urMemBufferCreate(context, UR_MEM_FLAG_READ_WRITE, 4096,
                                       nullptr, &buffer));
      ASSERT_NE(nullptr, buffer);
    }
  }

  void TearDown() override {
    for (const auto &buffer : buffers) {
      if (buffer) {
        EXPECT_SUCCESS(urMemRelease(buffer));
      }
    }
    if (kernel) {
      EXPECT_SUCCESS(urKernelRelease(kernel));
    }
    if (program) {
      EXPECT_SUCCESS(urProgramRelease(program));
    }
    uur::ContextTest::TearDown();
  }

  ur_program_handle_t program = nullptr;
  ur_kernel_handle_t kernel = nullptr;
  std::array<ur_mem_handle_t, 3> buffers;
};
UUR_INSTANTIATE_DEVICE_TEST_SUITE_P(urKernelSetArgMemObjTest);

TEST_P(urKernelSetArgMemObjTest, SuccessSingleArg) {
  ASSERT_SUCCESS(urKernelSetArgMemObj(kernel, 0, buffers[0]));
}

TEST_P(urKernelSetArgMemObjTest, SuccessMultiArgs) {
  for (unsigned arg_idx = 0; arg_idx < buffers.size(); ++arg_idx) {
    ASSERT_SUCCESS(urKernelSetArgMemObj(kernel, arg_idx, buffers[arg_idx]));
  }
}

TEST_P(urKernelSetArgMemObjTest, InvalidNullHandleKernel) {
  ASSERT_EQ_RESULT(UR_RESULT_ERROR_INVALID_NULL_HANDLE,
                   urKernelSetArgMemObj(nullptr, 0, buffers[0]));
}