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

#include "Common.h"

class clSVMFreeTest : public ucl::ContextTest {
 protected:
  void SetUp() override {
    UCL_RETURN_ON_FATAL_FAILURE(ucl::ContextTest::SetUp());
    if (!UCL::isDeviceVersionAtLeast({3, 0})) {
      GTEST_SKIP();
    }
  }
};

TEST_F(clSVMFreeTest, NotImplemented) {
  cl_device_svm_capabilities svm_capabilities{};
  ASSERT_SUCCESS(clGetDeviceInfo(device, CL_DEVICE_SVM_CAPABILITIES,
                                 sizeof(svm_capabilities), &svm_capabilities,
                                 nullptr));
  if (0 != svm_capabilities) {
    // Since we test against other implementations that may implement this
    // but we aren't actually testing the functionality, just skip.
    GTEST_SKIP();
  }
  void *svm_pointer{};
  clSVMFree(context, svm_pointer);
}
