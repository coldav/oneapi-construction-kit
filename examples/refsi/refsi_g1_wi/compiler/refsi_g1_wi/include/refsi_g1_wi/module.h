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

#ifndef REFSI_G1_MODULE_H_INCLUDED
#define REFSI_G1_MODULE_H_INCLUDED

#include <base/context.h>
#include <mux/mux.hpp>
#include <riscv/module.h>

namespace refsi_g1_wi {

class RefSiG1Target;

/// @brief A class that drives the compilation process and stores the compiled
/// binary.
class RefSiG1Module final : public riscv::RiscvModule {
 public:
  RefSiG1Module(RefSiG1Target &target, compiler::BaseContext &context,
                uint32_t &num_errors, std::string &log);

  /// @see Module::createPassMachinery
  std::unique_ptr<compiler::utils::PassMachinery> createPassMachinery(
      llvm::LLVMContext &) override;

  /// @see Module::getLateTargetPasses
  llvm::ModulePassManager getLateTargetPasses(
      compiler::utils::PassMachinery &pass_mach) override;

};  // class RefSiG1Module
}  // namespace refsi_g1_wi

#endif  // REFSI_G1_MODULE_H_INCLUDED
