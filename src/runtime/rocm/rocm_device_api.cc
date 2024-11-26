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
 * \file rocm_device_api.cc
 * \brief GPU specific API
 */
#include <dmlc/logging.h>
#include <dmlc/thread_local.h>
#include <hip/hip_runtime_api.h>
#include <hsa/hsa.h>
#include <tvm/runtime/device_api.h>
#include <tvm/runtime/registry.h>

#include "rocm_common.h"

namespace tvm {
namespace runtime {

class ROCMDeviceAPI final : public DeviceAPI {
 public:
  void SetDevice(TVMContext device) final {
    ROCM_CALL(hipSetDevice(device.device_id));
  }
  void GetAttr(TVMContext device, DeviceAttrKind kind, TVMRetValue* rv) final {
    int value = 0;
    switch (kind) {
      case kExist: {
        if (hsa_init() == HSA_STATUS_SUCCESS) {
          int dev;
          ROCM_CALL(hipGetDeviceCount(&dev));
          value = dev > device.device_id ? 1 : 0;
          hsa_shut_down();
        } else {
          value = 0;
        }
        break;
      }
      case kMaxThreadsPerBlock: {
        ROCM_CALL(hipDeviceGetAttribute(
            &value, hipDeviceAttributeMaxThreadsPerBlock, device.device_id));
        break;
      }
      case kWarpSize: {
        ROCM_CALL(hipDeviceGetAttribute(&value, hipDeviceAttributeWarpSize,
                                        device.device_id));
        break;
      }
      case kMaxSharedMemoryPerBlock: {
        ROCM_CALL(hipDeviceGetAttribute(
            &value, hipDeviceAttributeMaxSharedMemoryPerBlock, device.device_id));
        break;
      }
      case kComputeVersion: {
        std::ostringstream os;
        ROCM_CALL(hipDeviceGetAttribute(
            &value, hipDeviceAttributeComputeCapabilityMajor, device.device_id));
        os << value << ".";
        ROCM_CALL(hipDeviceGetAttribute(
            &value, hipDeviceAttributeComputeCapabilityMinor, device.device_id));
        os << value;
        *rv = os.str();
        return;
      }
      case kDeviceName:
        return;
      case kMaxClockRate: {
        ROCM_CALL(hipDeviceGetAttribute(&value, hipDeviceAttributeClockRate,
                                        device.device_id));
        break;
      }
      case kMultiProcessorCount: {
        ROCM_CALL(hipDeviceGetAttribute(
            &value, hipDeviceAttributeMultiprocessorCount, device.device_id));
        break;
      }
      case kMaxThreadDimensions: {
        int dims[3];
        ROCM_CALL(hipDeviceGetAttribute(
            &dims[0], hipDeviceAttributeMaxBlockDimX, device.device_id));
        ROCM_CALL(hipDeviceGetAttribute(
            &dims[1], hipDeviceAttributeMaxBlockDimY, device.device_id));
        ROCM_CALL(hipDeviceGetAttribute(
            &dims[2], hipDeviceAttributeMaxBlockDimZ, device.device_id));

        std::stringstream ss;
        ss << "[" << dims[0] << ", " << dims[1] << ", " << dims[2] << "]";
        *rv = ss.str();
        return;
      }
      case kGcnArch: {
        hipDeviceProp_t prop;
        ROCM_CALL(hipGetDeviceProperties(&prop, device.device_id));
        *rv = prop.gcnArch;
        return;
      }
    }
    *rv = value;
  }
  void* AllocDataSpace(TVMContext device, size_t nbytes, size_t alignment,
                       TVMType type_hint) final {
    ROCM_CALL(hipSetDevice(device.device_id));
    CHECK_EQ(256 % alignment, 0U) << "ROCM space is aligned at 256 bytes";
    void* ret;
    ROCM_CALL(hipMalloc(&ret, nbytes));
    return ret;
  }

  void FreeDataSpace(TVMContext device, void* ptr) final {
    ROCM_CALL(hipSetDevice(device.device_id));
    ROCM_CALL(hipFree(ptr));
  }

  void CopyDataFromTo(const void* from, size_t from_offset, void* to,
                      size_t to_offset, size_t size, TVMContext device_from,
                      TVMContext device_to, TVMType type_hint,
                      TVMStreamHandle stream) final {
    hipStream_t hip_stream = static_cast<hipStream_t>(stream);
    from = static_cast<const char*>(from) + from_offset;
    to = static_cast<char*>(to) + to_offset;
    if (device_from.device_type == kDLROCM && device_to.device_type == kDLROCM) {
      ROCM_CALL(hipSetDevice(device_from.device_id));
      if (device_from.device_id == device_to.device_id) {
        GPUCopy(from, to, size, hipMemcpyDeviceToDevice, hip_stream);
      } else {
        hipMemcpyPeerAsync(to, device_to.device_id, from, device_from.device_id, size,
                           hip_stream);
      }
    } else if (device_from.device_type == kDLROCM &&
               device_to.device_type == kDLCPU) {
      ROCM_CALL(hipSetDevice(device_from.device_id));
      GPUCopy(from, to, size, hipMemcpyDeviceToHost, hip_stream);
    } else if (device_from.device_type == kDLCPU &&
               device_to.device_type == kDLROCM) {
      ROCM_CALL(hipSetDevice(device_to.device_id));
      GPUCopy(from, to, size, hipMemcpyHostToDevice, hip_stream);
    } else {
      LOG(FATAL) << "expect copy from/to GPU or between GPU";
    }
  }

  void StreamSync(TVMContext device, TVMStreamHandle stream) final {
    ROCM_CALL(hipSetDevice(device.device_id));
    ROCM_CALL(hipStreamSynchronize(static_cast<hipStream_t>(stream)));
  }

  void SetStream(TVMContext device, TVMStreamHandle stream) final {
    ROCMThreadEntry::ThreadLocal()->stream = static_cast<hipStream_t>(stream);
  }

  void* AllocWorkspace(TVMContext device, size_t size, TVMType type_hint) final {
    return ROCMThreadEntry::ThreadLocal()->pool.AllocWorkspace(device, size);
  }

  void FreeWorkspace(TVMContext device, void* data) final {
    ROCMThreadEntry::ThreadLocal()->pool.FreeWorkspace(device, data);
  }

  static const std::shared_ptr<ROCMDeviceAPI>& Global() {
    static std::shared_ptr<ROCMDeviceAPI> inst =
        std::make_shared<ROCMDeviceAPI>();
    return inst;
  }

 private:
  static void GPUCopy(const void* from, void* to, size_t size,
                      hipMemcpyKind kind, hipStream_t stream) {
    if (stream != 0) {
      ROCM_CALL(hipMemcpyAsync(to, from, size, kind, stream));
    } else {
      ROCM_CALL(hipMemcpy(to, from, size, kind));
    }
  }
};

typedef dmlc::ThreadLocalStore<ROCMThreadEntry> ROCMThreadStore;

ROCMThreadEntry::ROCMThreadEntry() : pool(kDLROCM, ROCMDeviceAPI::Global()) {}

ROCMThreadEntry* ROCMThreadEntry::ThreadLocal() {
  return ROCMThreadStore::Get();
}

TVM_REGISTER_GLOBAL("device_api.rocm")
    .set_body([](TVMArgs args, TVMRetValue* rv) {
      DeviceAPI* ptr = ROCMDeviceAPI::Global().get();
      *rv = static_cast<void*>(ptr);
    });
}  // namespace runtime
}  // namespace tvm
