; Copyright (C) Codeplay Software Limited
;
; Licensed under the Apache License, Version 2.0 (the "License") with LLVM
; Exceptions; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;     https://github.com/codeplaysoftware/oneapi-construction-kit/blob/main/LICENSE.txt
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
; WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
; License for the specific language governing permissions and limitations
; under the License.
;
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

; RUN: veczc -vecz-simd-width=4 -S < %s | FileCheck %s 

target triple = "spir64-unknown-unknown"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"

declare spir_func i32 @_Z16get_sub_group_idv()
declare spir_func i32 @_Z22get_sub_group_local_idv()
declare spir_func i32 @_Z19sub_group_broadcastij(i32, i32)

; It makes sure broadcast still works when its source operand is uniform
define spir_kernel void @sub_group_broadcast(i32 addrspace(1)* %in, i32 addrspace(1)* %out) {
  %call = tail call spir_func i32 @_Z16get_sub_group_idv()
  %arrayidx = getelementptr inbounds i32, i32 addrspace(1)* %in, i32 %call
  %v = load i32, i32 addrspace(1)* %arrayidx, align 4
  %broadcast = call spir_func i32 @_Z19sub_group_broadcastij(i32 %v, i32 0)
  %idx = tail call spir_func i32 @_Z22get_sub_group_local_idv()
  %arrayidx2 = getelementptr inbounds i32, i32 addrspace(1)* %out, i32 %idx
  store i32 %broadcast, i32 addrspace(1)* %arrayidx2, align 4
  ret void
}

; CHECK-LABEL: define spir_kernel void @__vecz_v4_sub_group_broadcast(
; CHECK: [[LD:%.+]] = load i32, ptr addrspace(1) %{{.+}}, align 4
; CHECK: [[INS:%.+]] = insertelement <4 x i32> poison, i32 [[LD]], i64 0
; CHECK: [[BCAST:%.+]] = shufflevector <4 x i32> [[INS]], <4 x i32> poison, <4 x i32> zeroinitializer
; CHECK: store <4 x i32> [[BCAST]], ptr addrspace(1) %out, align 4

!opencl.ocl.version = !{!0}

!0 = !{i32 3, i32 0}
