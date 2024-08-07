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

  auto functionNeedsFixing = [&BI, &M](Function &F, bool &Vi2DoubleUpdate) {
    if (F.getName().starts_with("__mux_sub_group") ||
        F.getName().starts_with("__mux_work_group")) {
      for (auto &a : F.args()) {
        if (a.hasStructRetAttr() || a.hasByValAttr()) {
          return true;
        }
      }
      if (F.getName().contains("_v2i32")) {
        for (auto &a : F.args()) {
          if (a.getType()->isDoubleTy()) {
            Vi2DoubleUpdate = true;
            return true;
          }
        }
      }
    }
    return false;
  };

  llvm::SmallVector<Function *, 4> FuncsToProcess;
  // Define all mux builtins
  for (auto &F : M.functions()) {
    FuncsToProcess.push_back(&F);
  }

  for (auto *F : FuncsToProcess) {
    bool Vi2DoubleUpdate = false;
    if (!functionNeedsFixing(*F, Vi2DoubleUpdate)) {
      continue;
    }
    llvm::errs() << "**** CSD  Fix ABI mux builtin: " << *F << " : "
                 << Vi2DoubleUpdate << "\n";
    if (!F->isDeclaration()) {
      continue;
    }
    IRBuilder<> ir(BasicBlock::Create(F->getContext(), "", F));

    std::string OrigName = F->getName().str();
    F->setName(F->getName() + "_abi_wrapper");
    // auto NumArgs =
    //     F->arg_size();  // TODO: Calculate this correctly in case of sret
    // Create declaration that we expect
    // continue;
    llvm::SmallVector<Type *, 8> Args;  //(NumArgs);
    llvm::Argument *SretParam = nullptr;
    llvm::Type *RetType = F->getReturnType();
    for (auto &Arg : F->args()) {
      if (Arg.hasStructRetAttr()) {
        llvm::errs() << "**** CSD " << Arg.getName() << " is sret\n";
        SretParam = &Arg;
        RetType = SretParam->getParamStructRetType();
        continue;
      }
      if (Arg.hasByValAttr()) {
        Args.push_back(Arg.getParamByValType());
      } else if (Vi2DoubleUpdate && Arg.getType()->isDoubleTy()) {
        // We know this should not be a double type, so output an int2
        RetType = llvm::FixedVectorType::get(ir.getInt32Ty(), 2);
        Args.push_back(RetType);
      } else {
        Args.push_back(Arg.getType());
      }
    }

    FunctionType *FT = FunctionType::get(RetType, Args, false);
    Function *NewFunc = Function::Create(FT, F->getLinkage(), OrigName, M);
    (void)NewFunc;
    llvm::SmallVector<Value *, 8> CallArgs;
    auto NewFuncArgItr = NewFunc->args().begin();
    for (auto &Arg : F->args()) {
      if (Arg.hasByValAttr()) {
        Value *ArgLoad =
            ir.CreateLoad(Arg.getParamByValType(), &Arg);  //, Arg.getName());
        CallArgs.push_back(ArgLoad);
      } else if (Vi2DoubleUpdate && Arg.getType()->isDoubleTy()) {
        Value *ArgCast = ir.CreateBitCast(&Arg, (*NewFuncArgItr).getType());
        /* Value * 	CreateBitCast (Value *V, Type *DestTy, const Twine
         * &Name="")*/
        CallArgs.push_back(ArgCast);
      } else {
        CallArgs.push_back(&Arg);
      }
      NewFuncArgItr++;
    }

    llvm::errs() << "*** NewFunc = " << *NewFunc << "\n";
    for (auto &Arg : CallArgs) {
      llvm::errs() << "***       Arg = " << *Arg << "\n";
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
