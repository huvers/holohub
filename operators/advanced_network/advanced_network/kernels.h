/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <stdint.h>
#include <assert.h>
#include <cuda_runtime.h>

#if __cplusplus
extern "C" {
#endif

__attribute__((__visibility__("default"))) void simple_packet_reorder(void* out,
                                                                      const void* const* const in,
                                                                      uint16_t pkt_len,
                                                                      uint32_t num_pkts,
                                                                      cudaStream_t stream);
#if __cplusplus
}
#endif
