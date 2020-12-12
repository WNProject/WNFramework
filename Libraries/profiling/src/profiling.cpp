#include "profiling/inc/profiling.h"
#include <cstdlib>
namespace wn {
namespace profiling {

FrameMarker::FrameMarker(const char* _tag) : m_tag(_tag) {
  FrameMarkStart(m_tag);
}

FrameMarker::~FrameMarker() {
  FrameMarkEnd(m_tag);
}

bool is_profiling_enabled() {
#ifdef TRACY_ENABLE
  return true;
#else
  return false;
#endif
}

}  // namespace profiling
}  // namespace wn

#ifdef TRACY_ENABLE
static const char* default_allocator_name = "DefaultAllocator";

void* aligned_alloc(std::size_t size, std::align_val_t al) {
  al = std::max(al, static_cast<std::align_val_t>(alignof(void*)));

  std::size_t space = size + static_cast<size_t>(al) - 1;
  char* mem = static_cast<char*>(malloc(space));
  void* base = static_cast<void*>(mem + sizeof(void*));
  std::align(static_cast<size_t>(al), size, base, space);
  *(static_cast<void**>(base) - 1) = mem;
  return base;
}

void aligned_free(void* p) {
  free(*(static_cast<void**>(p) - 1));
}

void* operator new(std ::size_t count) {
  auto ptr = malloc(count);
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyAllocNS(ptr, count, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyAllocN(ptr, count, default_allocator_name);
#endif
  return ptr;
}
void* operator new[](std::size_t count) {
  auto ptr = std::malloc(count);
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyAllocNS(ptr, count, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyAllocN(ptr, count, default_allocator_name);
#endif
  return ptr;
}
void* operator new(std::size_t count, std::align_val_t al) {
  auto ptr = aligned_alloc(count, al);
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyAllocNS(ptr, count, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyAllocN(ptr, count, default_allocator_name);
#endif
  return ptr;
}
void* operator new[](std::size_t count, std::align_val_t al) {
  auto ptr = aligned_alloc(count, al);
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyAllocNS(ptr, count, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyAllocN(ptr, count, default_allocator_name);
#endif
  return ptr;
}

void operator delete(void* ptr) noexcept {
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyFreeNS(ptr, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyFreeN(ptr, default_allocator_name);
#endif
  free(ptr);
}
void operator delete[](void* ptr) noexcept {
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyFreeNS(ptr, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyFreeN(ptr, default_allocator_name);
#endif
  free(ptr);
}

void operator delete(void* ptr, std::align_val_t) noexcept {
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyFreeNS(ptr, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyFreeN(ptr, default_allocator_name);
#endif
  aligned_free(ptr);
}
void operator delete[](void* ptr, std::align_val_t) noexcept {
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyFreeNS(ptr, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyFreeN(ptr, default_allocator_name);
#endif
  aligned_free(ptr);
}
void operator delete(void* ptr, std::size_t) noexcept {
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyFreeNS(ptr, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyFreeN(ptr, default_allocator_name);
#endif
  free(ptr);
}
void operator delete[](void* ptr, std::size_t) noexcept {
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyFreeNS(ptr, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyFreeN(ptr, default_allocator_name);
#endif
  free(ptr);
}
void operator delete(void* ptr, std::size_t, std::align_val_t) noexcept {
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyFreeNS(ptr, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyFreeN(ptr, default_allocator_name);
#endif
  aligned_free(ptr);
}
void operator delete[](void* ptr, std::size_t, std::align_val_t) noexcept {
#if WN_TRACY_CALLBACK_DEPTH > 0
  TracyFreeNS(ptr, WN_TRACY_CALLBACK_DEPTH, default_allocator_name);
#else
  TracyFreeN(ptr, default_allocator_name);
#endif
  aligned_free(ptr);
}
#endif
