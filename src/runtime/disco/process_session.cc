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
#include <tvm/runtime/c_runtime_api.h>
#include <tvm/runtime/disco/disco_worker.h>
#include <tvm/runtime/object.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>

#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include "../../support/pipe.h"
#include "../minrpc/rpc_reference.h"
#include "./bcast_session.h"
#include "./disco_worker_thread.h"
#include "./protocol.h"

namespace tvm {
namespace runtime {

class DiscoPipeMessageQueue : private dmlc::Stream, private DiscoProtocol<DiscoPipeMessageQueue> {
 public:
  explicit DiscoPipeMessageQueue(int64_t handle) : pipe_(handle) {}

  ~DiscoPipeMessageQueue() = default;

  void Send(const TVMArgs& args) {
    RPCReference::ReturnPackedSeq(args.values, args.type_codes, args.num_args, this);
    CommitSendAndNotifyEnqueue();
  }

  TVMArgs Recv() {
    bool is_implicit_shutdown = DequeueNextPacket();
    TVMValue* values = nullptr;
    int* type_codes = nullptr;
    int num_args = 0;

    if (is_implicit_shutdown) {
      num_args = 2;
      values = ArenaAlloc<TVMValue>(num_args);
      type_codes = ArenaAlloc<int>(num_args);
      TVMArgsSetter setter(values, type_codes);
      setter(0, static_cast<int>(DiscoAction::kShutDown));
      setter(1, 0);
    } else {
      RPCReference::RecvPackedSeq(&values, &type_codes, &num_args, this);
    }
    return TVMArgs(values, type_codes, num_args);
  }

 protected:
  void CommitSendAndNotifyEnqueue() {
    pipe_.Write(write_buffer_.data(), write_buffer_.size());
    write_buffer_.clear();
  }

  /* \brief Read next packet and reset unpacker
   *
   * Read the next packet into `read_buffer_`, releasing all arena
   * allocations performed by the unpacker and resetting the unpacker
   * to its initial state.
   *
   * \return A boolean value.  If true, this packet should be treated
   *    equivalently to a `DiscoAction::kShutdown` event.  If false,
   *    this packet should be unpacked.
   */
  bool DequeueNextPacket() {
    uint64_t packet_nbytes = 0;
    int read_size = pipe_.Read(&packet_nbytes, sizeof(packet_nbytes));
    if (read_size == 0) {
      // Special case, connection dropped between packets.  Treat as a
      // request to shutdown.
      return true;
    }

 
