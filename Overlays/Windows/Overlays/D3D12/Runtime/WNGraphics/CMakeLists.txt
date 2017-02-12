append_to_overlay_var(WN_GRAPHICS_DEVICE_TYPES_ENABLED "D3D12")

overlay_library_sources(WNGraphics
  SOURCES
    inc/Internal/D3D12/WNAdapter.h
    inc/Internal/D3D12/WNCommandList.h
    inc/Internal/D3D12/WNDevice.h
    inc/Internal/D3D12/WNFenceData.h
    inc/Internal/D3D12/WNImageFormats.h
    inc/Internal/D3D12/WNQueue.h
    inc/Internal/D3D12/WNResourceStates.h
    inc/Internal/D3D12/WNSwapchain.h
    src/D3D12/WNAdapter.cpp
    src/D3D12/WNCommandList.cpp
    src/D3D12/WNDevice.cpp
    src/D3D12/WNHelpers.h
    src/D3D12/WNHelpers.cpp
    src/D3D12/WNQueue.cpp
    src/D3D12/WNSwapchain.cpp
  LIBS
    DXGI.lib D3D12.lib
)
