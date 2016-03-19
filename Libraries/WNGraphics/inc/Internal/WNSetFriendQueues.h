// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Intentionally no header guards, meant for multiple inclusion.
#ifdef _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
friend class wn::graphics::internal::d3d12::queue;
#endif
#ifdef _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
friend class wn::graphics::internal::vulkan::queue;
#endif