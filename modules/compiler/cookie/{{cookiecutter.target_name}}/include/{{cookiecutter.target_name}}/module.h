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
{% if cookiecutter.copyright_name != "" -%}
/// Additional changes Copyright (C) {{cookiecutter.copyright_name}}. All Rights
/// Reserved.
{% endif -%}

#ifndef {{cookiecutter.target_name_capitals}}_MODULE_H_INCLUDED
#define {{cookiecutter.target_name_capitals}}_MODULE_H_INCLUDED

#include <base/context.h>
#include <base/module.h>
#include <cargo/array_view.h>
#include <cargo/dynamic_array.h>
#include <cargo/optional.h>
#include <mux/mux.hpp>

namespace {{cookiecutter.target_name}} {

class {{cookiecutter.target_name.capitalize()}}Target;

/// @brief A class that drives the compilation process and stores the compiled
/// binary.
class {{cookiecutter.target_name.capitalize()}}Module final : public compiler::BaseModule {
 public:
  {{cookiecutter.target_name.capitalize()}}Module({{cookiecutter.target_name.capitalize()}}Target &target, compiler::BaseContext &context, uint32_t &num_errors,
         std::string &log);

  /// @see Module::clear
  void clear() override;

  /// @see Module::createBinary
  compiler::Result createBinary(cargo::array_view<std::uint8_t> & buffer)
      override;

  /// @see Module::createPassMachinery
  std::unique_ptr<compiler::utils::PassMachinery> createPassMachinery(
      llvm::LLVMContext &) override;

  /// @see BaseModule::initializePassMachineryForFrontend
  void initializePassMachineryForFrontend(compiler::utils::PassMachinery &,
                                          const clang::CodeGenOptions &)
      const override;

  /// @see BaseModule::initializePassMachineryForFinalize
  void initializePassMachineryForFinalize(compiler::utils::PassMachinery &)
      const override;

  /// @brief Stores the metadata for a kernel.
  struct KernelMetadata {
    std::string name;
    uint32_t local_memory_used;
    uint32_t subgroup_size;
  };

  /// @brief Gets or creates the TargetMachine to be used in the compilation of
  /// this module.
  llvm::TargetMachine *getTargetMachine();

 protected:
  /// @see BaseModule::getLateTargetPasses
  llvm::ModulePassManager getLateTargetPasses(compiler::utils::PassMachinery &)
      override;

  /// @see Module::createKernel
  compiler::Kernel *createKernel(const std::string &name) override;

  const {{cookiecutter.target_name.capitalize()}}Target &getTarget() const;

 private:
  cargo::dynamic_array<uint8_t> object_code;

  /// @brief Target machine to use to compile IR to assembly.
  std::unique_ptr<llvm::TargetMachine> target_machine;
};  // class {{cookiecutter.target_name.capitalize()}}Module
}  // namespace {{cookiecutter.target_name}}

#endif  // {{cookiecutter.target_name_capitals}}_MODULE_H_INCLUDED
