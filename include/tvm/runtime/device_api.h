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
 * \file tvm/runtime/device_api.h
 * \brief Abstract device memory management API
 */
#ifndef TVM_RUNTIME_DEVICE_API_H_
#define TVM_RUNTIME_DEVICE_API_H_

#include <string>
#include "packed_func.h"
#include "c_runtime_api.h"

namespace tvm {
namespace runtime {
/*!
 * \brief the query type into GetAttr
 */
enum DeviceAttrKind : int {
  kExist = 0,
  kMaxThreadsPerBlock = 1,
  kWarpSize = 2,
  kMaxSharedMemoryPerBlock = 3,
  kComputeVersion = 4,
  kDeviceName = 5,
  kMaxClockRate = 6,
  kMultiProcessorCount = 7,
  kMaxThreadDimensions = 8,
  kGcnArch = 9
};

/*! \brief Number of bytes each allocation must align to */
constexpr int kAllocAlignment = 64;

/*! \brief Number of bytes each allocation must align to in temporary allocation */
constexpr int kTempAllocaAlignment = 64;

/*! \brief Maximum size that can be allocated on stack */
constexpr int kMaxStackAlloca = 1024;

/*!
 *  \brief TVM Runtime Device API, abstracts the device
 *  specific interface for memory management.
 */
class TVM_DLL DeviceAPI {
 public:
  /*! \brief virtual destructor */
  virtual ~DeviceAPI() {}
  /*!
   * \brief Set the environment device id to device
   * \param device The context to be set.
   */
  virtual void SetDevice(TVMContext device) = 0;
  /*!
   * \brief Get attribute of specified device.
   * \param device The device context
   * \param kind The result kind
   * \param rv The return value.
   * \sa DeviceAttrKind
   */
  virtual void GetAttr(TVMContext device, DeviceAttrKind kind, TVMRetValue* rv) = 0;
  /*!
   * \brief Allocate a data space on device.
   * \param device The device context to perform operation.
   * \param nbytes The number of bytes in memory.
   * \param alignment The alignment of the memory.
   * \param type_hint The type of elements. Only needed by certain backends such
   * as OpenGL, as nbytes & alignment are sufficient for most backends.
   * \return The allocated device pointer.
   */
  virtual void* AllocDataSpace(TVMContext device,
                               size_t nbytes,
                               size_t alignment,
                               TVMType type_hint) = 0;
  /*!
   * \brief Free a data space on device.
   * \param device The device context to perform operation.
   * \param ptr The data space.
   */
  virtual void FreeDataSpace(TVMContext device, void* ptr) = 0;
  /*!
   * \brief copy data from one place to another
   * \param from The source array.
   * \param from_offset The byte offeset in the from.
   * \param to The target array.
   * \param to_offset The byte offset in the to.
   * \param num_bytes The size of the memory in bytes
   * \param device_from The source context
   * \param device_to The target context
   * \param type_hint The type of elements, only neded by certain backends.
   *                  can be useful for cross device endian converison.
   * \param stream Optional stream object.
   */
  virtual void CopyDataFromTo(const void* from,
                              size_t from_offset,
                              void* to,
                              size_t to_offset,
                              size_t num_bytes,
                              TVMContext device_from,
                              TVMContext device_to,
                              TVMType type_hint,
                              TVMStreamHandle stream) = 0;
    /*!
   * \brief Create a new stream of execution.
   *
   * \param device The context of allocation.
   */
  virtual TVMStreamHandle CreateStream(TVMContext device);

  /*!
   * \brief Free a stream of execution
   *
   * \param device The context of the stream
   * \param stream The pointer to be freed.
   */
  virtual void FreeStream(TVMContext device, TVMStreamHandle stream);

  /*!
   * \brief Synchronize the stream
   * \param device The context to perform operation.
   * \param stream The stream to be sync.
   */
  virtual void StreamSync(TVMContext device, TVMStreamHandle stream) = 0;
  /*!
   * \brief Set the stream
   * \param device The context to set stream.
   * \param stream The stream to be set.
   */
  virtual void SetStream(TVMContext device, TVMStreamHandle stream) {}
  /*!
   * \brief Synchronize 2 streams of execution.
   *
   * An event is created in event_src stream that the second then
   * stream waits on.  Neither event_src or event_dst need to be of
   * the same device ID as the context, but they must be of the same
   * device type.
   *
   * \param device The context of the streams.
   * \param event_src The source stream to synchronize.
   * \param event_dst The destination stream to synchronize.
   */
  virtual void SyncStreamFromTo(TVMContext device,
                                        TVMStreamHandle event_src,
                                        TVMStreamHandle event_dst);
  /*!
   * \brief Allocate temporal workspace for backend execution.
   *
   *  \note We have the following assumption about backend temporal
   *   workspace allocation, and backend will optimize for such assumption:
   *
   *  - Only a few allocation will happen, and space will be released after use.
   *  - The release order is usually in reverse order of allocate (stack style).
   *  - Repeative pattern of same allocations over different runs.
   *  - Workspace should not overlap between different threads(i.e. be threadlocal)
   *
   * \param device The context of allocation.
   * \param nbytes The size to be allocated.
   * \param type_hint The type of elements. Only needed by certain backends such
   * as OpenGL, as nbytes is sufficient for most backends.
   */
  virtual void* AllocWorkspace(TVMContext device,
                                       size_t nbytes,
                                       TVMType type_hint = {});
  /*!
   * \brief Free temporal workspace in backend execution.
   *
   * \param device The context of allocation.
   * \param ptr The pointer to be freed.
   */
  virtual void FreeWorkspace(TVMContext device, void* ptr);

  /*!
   * \brief Get device API base don context.
   * \param device The context
   * \param allow_missing Whether allow missing
   * \return The corresponding device API.
   */
  static DeviceAPI* Get(TVMContext device, bool allow_missing = false);
};

/*! \brief The device type bigger than this is RPC device */
constexpr int kRPCSessMask = 128;

/*!
 * \brief The name of Device API factory.
 * \param type The device type.
 * \return the device name.
 */
inline const char* DeviceName(int type) {
  switch (type) {
    case kDLCPU: return "cpu";
    case kDLGPU: return "gpu";
    case kDLOpenCL: return "opencl";
    case kDLSDAccel: return "sdaccel";
    case kDLAOCL: return "aocl";
    case kDLVulkan: return "vulkan";
    case kDLMetal: return "metal";
    case kDLVPI: return "vpi";
    case kDLROCM: return "rocm";
    case kOpenGL: return "opengl";
    case kDLExtDev: return "ext_dev";
    case kDLMicroDev: return "micro_dev";
    default: LOG(FATAL) << "unknown type =" << type; return "Unknown";
  }
}

#ifndef _LIBCPP_SGX_NO_IOSTREAMS
inline std::ostream& operator<<(std::ostream& os, DLDevice device) {  // NOLINT(*)
  int device_type = static_cast<int>(device.device_type);
  if (device_type > kRPCSessMask) {
    os << "remote[" << (device_type / kRPCSessMask) << "]-";
    device_type = device_type % kRPCSessMask;
  }
  os << runtime::DeviceName(device_type) << "(" << device.device_id << ")";
  return os;
}

#endif
}  // namespace runtime
}  // namespace tvm
#endif  // TVM_RUNTIME_DEVICE_API_H_
