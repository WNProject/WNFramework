// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/src/D3D12/WNHelpers.h"
#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/Internal/D3D12/WNAdapter.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"

#include <d3d12.h>
#include <dxgi1_4.h>

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {
namespace {

const uint32_t MICROSOFT_VENDOR = 0x1414;
const uint32_t BASIC_RENDER_DEVICE = 140;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_adapter_constructable = d3d12_adapter;
#else
using d3d12_adapter_constructable = adapter;
#endif

WN_INLINE bool convert_to_utf8(
    LPCWSTR _buffer, const DWORD _buffer_size, containers::string& _path) {
  const int buffer_size = static_cast<const int>(_buffer_size);
  int converted_temp_path_size = ::WideCharToMultiByte(
      CP_UTF8, 0, _buffer, buffer_size, NULL, 0, NULL, NULL);

  if (converted_temp_path_size != 0) {
    containers::string path(static_cast<size_t>(converted_temp_path_size), 0);

    converted_temp_path_size = ::WideCharToMultiByte(CP_UTF8, 0, _buffer,
        buffer_size, &path[0], static_cast<const int>(path.size()), NULL, NULL);

    if (converted_temp_path_size != 0) {
      _path = std::move(path);

      return true;
    }
  }

  return false;
}

bool determine_support(memory::allocator* _allocator, logging::log* _log,
    const size_t _device_number, const bool _allow_software_devices,
    IDXGIAdapter1* _dxgi_adapter, containers::string* _name,
    uint32_t* _device_id, uint32_t* _vendor_id) {
  // This is set to D3D_FEATURE_LEVEL_11_0 because this is the lowest posible
  // d3d version d3d12 supports.  This allows us to scale up on device
  // capabilities and work on older hardware
  HRESULT hr = ::D3D12CreateDevice(
      _dxgi_adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);

  if (FAILED(hr)) {
    _log->log_warning("Adapter does not have D3D12 hardware support, hr: ", hr);

    return false;
  }

  DXGI_ADAPTER_DESC1 dxgi_adapter_desc;

  hr = _dxgi_adapter->GetDesc1(&dxgi_adapter_desc);

  if (FAILED(hr)) {
    _log->log_error("Could not get DXGI adapter description, hr: ", hr);

    return false;
  }

  containers::string name(_allocator);

  if (!convert_to_utf8(dxgi_adapter_desc.Description,
          static_cast<DWORD>(::wcslen(dxgi_adapter_desc.Description)), name)) {
    _log->log_error("Could not convert adapter name to utf-8");

    return false;
  }

  if (!_allow_software_devices) {
    if (dxgi_adapter_desc.DeviceId == BASIC_RENDER_DEVICE &&
        dxgi_adapter_desc.VendorId == MICROSOFT_VENDOR) {
      // If this is the "Basic Renderer Driver" and we are not allowing
      // reference devices then ignore it. We do not want to present that to the
      // user.
      return false;
    }
  }

  _log->log_info("D3D12 Device: ", _device_number + 1);
  _log->log_info(" - Name: ", name.c_str());
  _log->log_info(" - Vendor: ", dxgi_adapter_desc.DeviceId);
  _log->log_info(" - Device: ", dxgi_adapter_desc.VendorId);

  (*_name) = core::move(name);
  (*_device_id) = static_cast<uint32_t>(dxgi_adapter_desc.DeviceId);
  (*_vendor_id) = static_cast<uint32_t>(dxgi_adapter_desc.VendorId);

  return true;
}

}  // anonymous namespace

void enumerate_adapters(memory::allocator* _allocator, logging::log* _log,
    containers::dynamic_array<adapter_ptr>& _physical_devices) {
  _log->log_info("Enumerating D3D12 Dvices");

  Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi_factory;
  HRESULT hr = ::CreateDXGIFactory1(__uuidof(IDXGIFactory4), &dxgi_factory);

  if (FAILED(hr)) {
    _log->log_error("Could not to create DXGI Factory, hr: ", hr);

    return;
  }

  UINT i = 0;

  for (;; ++i) {
    Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_adapter;

    hr = dxgi_factory->EnumAdapters1(i, &dxgi_adapter);

    if (hr == DXGI_ERROR_NOT_FOUND) {
      _log->log_info("Finished Enumerating D3D12 Dvices");

      break;
    } else if (FAILED(hr)) {
      _log->log_error("Could not query for DXGI adapter, hr: ", hr);

      continue;
    }

    uint32_t device_id;
    uint32_t vendor_id;
    containers::string name(_allocator);

    if (determine_support(_allocator, _log, i, false, dxgi_adapter.Get(), &name,
            &device_id, &vendor_id)) {
      memory::unique_ptr<d3d12_adapter_constructable> ptr(
          memory::make_unique_delegated<d3d12_adapter_constructable>(
              _allocator, [](void* _memory) {
                return new (_memory) d3d12_adapter_constructable();
              }));

      if (ptr) {
        ptr->initialize(core::move(dxgi_adapter), dxgi_factory,
            core::move(name), device_id, vendor_id);
      }

      _physical_devices.push_back(core::move(ptr));
    }
  }

#ifdef _WN_GRAPHICS_ALLOW_REFERENCE_DEVICES
  if (_physical_devices.empty()) {
    // create warp device
    Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_warp_adapter;

    hr = dxgi_factory->EnumWarpAdapter(
        __uuidof(IDXGIAdapter1), &dxgi_warp_adapter);

    if (hr == DXGI_ERROR_NOT_FOUND) {
      _log->log_info("Finished Enumerating D3D12 Dvices");

      return;
    } else if (FAILED(hr)) {
      _log->log_error("Could not query for warp adapter, hr: ", hr);

      return;
    }

    uint32_t device_id;
    uint32_t vendor_id;
    containers::string name(_allocator);

    if (determine_support(_allocator, _log, i, true, dxgi_warp_adapter.Get(),
            &name, &device_id, &vendor_id)) {
      memory::unique_ptr<d3d12_adapter_constructable> ptr(
          memory::make_unique_delegated<d3d12_adapter_constructable>(
              _allocator, [](void* _memory) {
                return new (_memory) d3d12_adapter_constructable();
              }));

      if (ptr) {
        ptr->initialize(core::move(dxgi_warp_adapter), dxgi_factory,
            core::move(name), device_id, vendor_id);
      }

      _physical_devices.push_back(core::move(ptr));
    }
  }
#endif
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn
