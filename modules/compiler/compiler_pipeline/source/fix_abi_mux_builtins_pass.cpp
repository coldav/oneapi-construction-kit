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

#include <compiler/utils/builtin_info.h>
#include <compiler/utils/fix_abi_mux_builtins_pass.h>
#include <llvm/IR/Module.h>

#define DEBUG_TYPE "fix-abi-mux-builtins"

using namespace llvm;

PreservedAnalyses compiler::utils::FixABIMuxBuiltinsPass::run(
    Module &M, ModuleAnalysisManager &AM) {
  bool Changed = false;
  auto &BI = AM.getResult<BuiltinInfoAnalysis>(M);

  auto functionNeedsFixing = [&BI, &M](Function &F) {
    if (F.getName().starts_with("__mux_sub_group") || F.getName().starts_with("__mux_work_group")) {
      for (auto &a : F.args()) {
        if (a.hasStructRetAttr() || a.hasByValAttr()) {
          return true;
        }
      }
    }
    return false;
  };

  // Define all mux builtins
  for (auto &F : M.functions()) {
    if (!functionNeedsFixing(F)) {
      continue;
    }
    // llvm::errs() << "  Fix ABI mux builtin: " << F.getName() << "\n";
    if (!F.isDeclaration()) {
      continue;
    }
    std::string OrigName = F.getName().str();
    F.setName(F.getName() + "_abi_wrapper");
    // auto NumArgs =
    //     F.arg_size();  // TODO: Calculate this correctly in case of sret
    // Create declaration that we expect
    // continue;
    llvm::SmallVector<Type *, 8> Args;  //(NumArgs);
    llvm::Argument *SretParam = nullptr;
    for (auto &Arg : F.args()) {
      if (Arg.hasStructRetAttr()) {
        llvm::errs() << "**** CSD " << Arg.getName()  << " is sret\n";
        SretParam = &Arg; 
        continue;
      }
      if (Arg.hasByValAttr()) {
        Args.push_back(Arg.getParamByValType());
      } else {
        Args.push_back(Arg.getType());
      }
    }
    llvm::Type *CalledFuncRetType = SretParam ? SretParam->getParamStructRetType() : F.getReturnType();
    if (SretParam) {
      llvm::errs() << "**** Need to do something about sret param" << *SretParam << "\n";
    }
    FunctionType *FT = FunctionType::get(CalledFuncRetType, Args, false);
    Function *NewFunc =
        Function::Create(FT, F.getLinkage(), OrigName, M);
    (void) NewFunc;
    IRBuilder<> ir(BasicBlock::Create(F.getContext(), "", &F));
    llvm::SmallVector<Value *, 8> CallArgs;
    for (auto &Arg : F.args()) {
      if (Arg.hasByValAttr()) {
        Value *ArgLoad =
            ir.CreateLoad(Arg.getParamByValType(), &Arg);  //, Arg.getName());
        CallArgs.push_back(ArgLoad);
      } else {
        CallArgs.push_back(&Arg);
      }
    }

    llvm::errs() << "*** NewFunc = " << *NewFunc <<"\n";
    for (auto &Arg : CallArgs) {
      llvm::errs() << "***       Arg = " << Arg <<"\n";
    }
    auto *Res = ir.CreateCall(NewFunc, CallArgs);
    if (SretParam) {
      // Store the return value
      ir.CreateStore(Res, SretParam);
      ir.CreateRetVoid();
    } else {
      ir.CreateRet(Res);
    }
    Changed = true;
  }

  return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
