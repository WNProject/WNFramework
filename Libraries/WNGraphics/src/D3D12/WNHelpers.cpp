// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/Internal/D3D12/WNPhysicalDevice.h"
#include "WNGraphics/inc/WNPhysicalDevice.h"
#include "WNGraphics/src/D3D12/WNHelpers.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"

#include <D3D12.h>
#include <DXGI.h>

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {
namespace {

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

}  // anonymous namespace

void enumerate_physical_devices(memory::allocator* _allocator,
    WNLogging::WNLog* _log,
    containers::dynamic_array<physical_device_ptr>& _physical_devices) {
  _log->Log(WNLogging::eInfo, 0, "Enumerating D3D12 Dvices");

  Microsoft::WRL::ComPtr<IDXGIFactory1> dxgi_factory;
  HRESULT hr = ::CreateDXGIFactory1(__uuidof(IDXGIFactory1), &dxgi_factory);

  if (FAILED(hr)) {
    _log->Log(
        WNLogging::eError, 0, "Could not to create DXGI Factory, hr: ", hr);

    return;
  }

  for (UINT i = 0;; ++i) {
    Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_adapter;

    hr = dxgi_factory->EnumAdapters1(i, &dxgi_adapter);

    if (hr == DXGI_ERROR_NOT_FOUND) {
      _log->Log(WNLogging::eInfo, 0, "Finished Enumerating D3D12 Dvices");

      break;
    } else if (FAILED(hr)) {
      _log->Log(
          WNLogging::eError, 0, "Could not query for DXGI adapter, hr: ", hr);

      continue;
    }

    // This is set to D3D_FEATURE_LEVEL_11_0 because this is the lowest posible
    // d3d version d3d12 supports.  This allows us to scale up on device
    // capabilities and work on older hardware
    hr = ::D3D12CreateDevice(dxgi_adapter.Get(), D3D_FEATURE_LEVEL_11_0,
        __uuidof(ID3D12Device), nullptr);

    if (FAILED(hr)) {
      _log->Log(
          WNLogging::eError, 0, "Could not determine D3D12 support, hr: ", hr);

      continue;
    }

    DXGI_ADAPTER_DESC1 dxgi_adapter_desc;

    hr = dxgi_adapter->GetDesc1(&dxgi_adapter_desc);

    if (FAILED(hr)) {
      _log->Log(WNLogging::eError, 0,
          "Could not get DXGI adapter description, hr: ", hr);

      continue;
    }

    containers::string name(_allocator);

    if (!convert_to_utf8(dxgi_adapter_desc.Description,
            static_cast<DWORD>(::wcslen(dxgi_adapter_desc.Description)),
            name)) {
      _log->Log(
          WNLogging::eError, 0, "Could not convert adapter name to utf-8");

      continue;
    }

    _log->Log(WNLogging::eInfo, 0, "D3D12 Device: ", i + 1);
    _log->Log(WNLogging::eInfo, 0, "------------------------------");
    _log->Log(WNLogging::eInfo, 0, "Name: ", name.c_str());
    _log->Log(WNLogging::eInfo, 0, "Vendor: ", dxgi_adapter_desc.DeviceId);
    _log->Log(WNLogging::eInfo, 0, "Device: ", dxgi_adapter_desc.VendorId);
    _log->Log(WNLogging::eInfo, 0, "------------------------------");

    memory::unique_ptr<physical_device> phys_device(
        memory::make_unique<internal::d3d12::physical_device>(_allocator,
            core::move(dxgi_adapter), core::move(name),
            static_cast<uint32_t>(dxgi_adapter_desc.DeviceId),
            static_cast<uint32_t>(dxgi_adapter_desc.VendorId)));

    _physical_devices.push_back(std::move(phys_device));
  }
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn
