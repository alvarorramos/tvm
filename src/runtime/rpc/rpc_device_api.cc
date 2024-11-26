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
 * \file rpc_device_api.cc
 */
#include <dmlc/logging.h>
#include <tvm/runtime/registry.h>
#include <tvm/runtime/device_api.h>
#include "rpc_session.h"

namespace tvm {
namespace runtime {

class RPCDeviceAPI final : public DeviceAPI {
 public:
  void SetDevice(TVMContext device) final {
    GetSess(device)->CallRemote(
        RPCCode::kDevSetDevice, device);
  }
  void GetAttr(TVMContext device, DeviceAttrKind kind, TVMRetValue* rv) final {
    *rv = GetSess(device)->CallRemote(
        RPCCode::kDevGetAttr, device, static_cast<int>(kind));
  }
  void* AllocDataSpace(TVMContext device,
                       size_t nbytes,
                       size_t alignment,
                       TVMType type_hint) final {
    auto sess = GetSess(device);
    void *data = sess->CallRemote(
            RPCCode::kDevAllocData, device, nbytes, alignment, type_hint);
    RemoteSpace* space = new RemoteSpace();
    space->data = data;
    space->sess = std::move(sess);
    return space;
  }
  void FreeDataSpace(TVMContext device, void* ptr) final {
    RemoteSpace* space = static_cast<RemoteSpace*>(ptr);
    try {
      GetSess(device)->CallRemote(
          RPCCode::kDevFreeData, device, space->data);
    } catch (const dmlc::Error& e) {
      // fault tolerance to remote close.
    }
    delete space;
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
    int from_dev_type = device_from.device_type;
    int to_dev_type = device_to.device_type;
    if (from_dev_type > kRPCSessMask &&
        to_dev_type > kRPCSessMask) {
      CHECK(device_from.device_type == device_to.device_type)
          << "Cannot copy across two different remote session";
      GetSess(device_from)->CallRemote(
          RPCCode::kCopyAmongRemote,
          static_cast<const RemoteSpace*>(from)->data, from_offset,
          static_cast<const RemoteSpace*>(to)->data, to_offset,
          size,  device_from, device_to, type_hint, stream);
    } else if (from_dev_type > kRPCSessMask &&
               to_dev_type == kDLCPU) {
      GetSess(device_from)->CopyFromRemote(
          static_cast<const RemoteSpace*>(from)->data, from_offset,
          to, to_offset, size, device_from, type_hint);
    } else if (from_dev_type == kDLCPU &&
               to_dev_type > kRPCSessMask) {
      GetSess(device_to)->CopyToRemote(
          (void*)from, from_offset,  // NOLINT(*)
          static_cast<const RemoteSpace*>(to)->data, to_offset,
          size, device_to, type_hint);
    } else {
      LOG(FATAL) << "expect copy from/to remote or between remote";
    }
  }
  void StreamSync(TVMContext device, TVMStreamHandle stream) final {
    GetSess(device)->CallRemote(
        RPCCode::kDevStreamSync, device, stream);
  }

 private:
  std::shared_ptr<RPCSession> GetSess(TVMContext device) {
    int dev_type = device.device_type;
    CHECK_GE(dev_type, kRPCSessMask);
    int tbl_index = dev_type / kRPCSessMask -  1;
    return RPCSession::Get(tbl_index);
  }
};

TVM_REGISTER_GLOBAL("device_api.rpc")
.set_body([](TVMArgs args, TVMRetValue* rv) {
    static RPCDeviceAPI inst;
    DeviceAPI* ptr = &inst;
    *rv = static_cast<void*>(ptr);
  });
}  // namespace runtime
}  // namespace tvm
