/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*!
 * \file device_api.cc
 * \brief TVM device API for VTA
 */

#include <tvm/runtime/registry.h>
#include <dmlc/thread_local.h>
#include <vta/runtime.h>

#include "../../src/runtime/workspace_pool.h"


namespace tvm {
namespace runtime {

class VTADeviceAPI final : public DeviceAPI {
 public:
  void SetDevice(TVMContext device) final {}

  void GetAttr(TVMContext device, DeviceAttrKind kind, TVMRetValue* rv) final {
    if (kind == kExist) {
      *rv = 1;
    }
  }

  void* AllocDataSpace(TVMContext device,
                       size_t size,
                       size_t alignment,
                       TVMType type_hint) final {
    return VTABufferAlloc(size);
  }

  void FreeDataSpace(TVMContext device, void* ptr) final {
    VTABufferFree(ptr);
  }

  void CopyDataFromTo(const void* from,
                      size_t from_offset,
                      void* to,
                      size_t to_offset,
                      size_t size,
                      TVMContext device_from,
                      TVMContext device_to,
                      TVMType type_hint,
                      TVMStreamHandle stream) final {
    int kind_mask = 0;
    if (device_from.device_type != kDLCPU) {
      kind_mask |= 2;
    }
    if (device_to.device_type != kDLCPU) {
      kind_mask |= 1;
    }
    VTABufferCopy(from, from_offset,
                  to, to_offset,
                  size, kind_mask);
  }

  void StreamSync(TVMContext device, TVMStreamHandle stream) final {
  }

  void* AllocWorkspace(TVMContext device, size_t size, TVMType type_hint) final;

  void FreeWorkspace(TVMContext device, void* data) final;

  static const std::shared_ptr<VTADeviceAPI>& Global() {
    static std::shared_ptr<VTADeviceAPI> inst =
        std::make_shared<VTADeviceAPI>();
    return inst;
  }
};

struct VTAWorkspacePool : public WorkspacePool {
  VTAWorkspacePool() :
      WorkspacePool(kDLExtDev, VTADeviceAPI::Global()) {}
};

void* VTADeviceAPI::AllocWorkspace(TVMContext device, size_t size, TVMType type_hint) {
  return dmlc::ThreadLocalStore<VTAWorkspacePool>::Get()
      ->AllocWorkspace(device, size);
}

void VTADeviceAPI::FreeWorkspace(TVMContext device, void* data) {
  dmlc::ThreadLocalStore<VTAWorkspacePool>::Get()->FreeWorkspace(device, data);
}

// Register device api with override.
static TVM_ATTRIBUTE_UNUSED auto& __register_dev__ =
::tvm::runtime::Registry::Register("device_api.ext_dev", true)
.set_body([](TVMArgs args, TVMRetValue* rv) {
    DeviceAPI* ptr = VTADeviceAPI::Global().get();
    *rv = static_cast<void*>(ptr);
  });
}  // namespace runtime
}  // namespace tvm
