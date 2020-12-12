#include "profiling/inc/allocator.h"
#include "Tracy.hpp"

namespace wn {
namespace profiling {

#if defined(WN_TRACY_CALLBACK_DEPTH) && WN_TRACY_CALLBACK_DEPTH > 0
#define TRACY_ALLOC(p, s, n) TracyAllocNS(p, s, WN_TRACY_CALLBACK_DEPTH, n)
#define TRACY_FREE(p, n) TracyFreeNS(p, WN_TRACY_CALLBACK_DEPTH, n)
#else
#define TRACY_ALLOC(p, s, n) TracyAllocN(p, s, n)
#define TRACY_FREE(p, n) TracyFreeN(p, n)
#endif

void* allocator::allocate(size_t _size) {
  void* ret = m_allocator->allocate(_size);
  TRACY_ALLOC(ret, _size, m_allocator_name)
  return ret;
}

void* allocator::reallocate(void* _ptr, size_t _new_size) {
  TRACY_FREE(_ptr, m_allocator_name)
  void* ret = m_allocator->reallocate(_ptr, _new_size);
  TRACY_ALLOC(ret, _new_size, m_allocator_name)
  return ret;
}

void allocator::deallocate(void* _ptr) {
  TRACY_FREE(_ptr, m_allocator_name)
  m_allocator->deallocate(_ptr);
}

void* allocator::aligned_allocate(size_t _size, size_t _alignment) {
  void* ret = m_allocator->aligned_allocate(_size, _alignment);
  TRACY_ALLOC(ret, _size, m_allocator_name)
  return ret;
}

void* allocator::aligned_reallocate(
    void* _ptr, size_t _new_size, size_t _alignment) {
  TRACY_FREE(_ptr, m_allocator_name)
  void* ret = m_allocator->aligned_reallocate(_ptr, _new_size, _alignment);
  TRACY_ALLOC(ret, _new_size, m_allocator_name)
  return ret;
}

void allocator::aligned_deallocate(void* _ptr) {
  TRACY_FREE(_ptr, m_allocator_name)
  m_allocator->aligned_deallocate(_ptr);
}

}  // namespace profiling
}  // namespace wn
