#include "WNGraphics/inc/Internal/D3D12/WNPhysicalDevice.h"

#include <DXGI.h>

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

physical_device::~physical_device() {
  if (m_adapter) {
    m_adapter->Release();
  }
}

}  // namespace d3d12
}  // namespace graphics
}  // namespace internal
}  // namespace d3d12