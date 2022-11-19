// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/inc/ui_rocket_interop.h"

#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNLog.h"
#include "WNWindow/inc/WNWindow.h"

namespace {

wn::containers::string_view get_string_view(const Rml::String& string) {
  return wn::containers::string_view(string.c_str());
}

struct ui_file {
  wn::file_system::file_ptr m_file;
  size_t m_offset;
};

}  // namespace

namespace wn {
namespace engine {
namespace ui {

rocket_system_interface::rocket_system_interface(
    wn::logging::log* _log, wn::runtime::window::window* _window)
  : m_log(_log),
    m_start(std::chrono::high_resolution_clock::now()),
    m_window(_window) {}

bool rocket_system_interface::LogMessage(
    Rml::Log::Type type, const Rml::String& message) {
  switch (type) {
    case Rml::Log::LT_ALWAYS:
      m_log->log_critical(message.c_str());
      break;
    case Rml::Log::LT_ASSERT:
      m_log->log_critical("ROCKET_ASSERT: ", message.c_str());
      break;
    case Rml::Log::LT_DEBUG:
      m_log->log_debug(message.c_str());
      break;
    case Rml::Log::LT_ERROR:
      m_log->log_error(message.c_str());
      break;
    case Rml::Log::LT_INFO:
      m_log->log_info(message.c_str());
      break;
    case Rml::Log::LT_WARNING:
      m_log->log_warning(message.c_str());
      break;
    case Rml::Log::LT_MAX:
      m_log->log_critical(message.c_str());
      break;
  }
  return true;
}

double rocket_system_interface::GetElapsedTime() {
  double elapsed_time =
      std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(
          std::chrono::high_resolution_clock::now() - m_start)
          .count();
  return elapsed_time;
}

// Activate keyboard (for touchscreen devices)
void rocket_system_interface::ActivateKeyboard(
    Rml::Vector2f caret_position, float line_height) {
  (void)caret_position;
  (void)line_height;
  m_window->show_keyboard();
}

// Deactivate keyboard (for touchscreen devices)
void rocket_system_interface::DeactivateKeyboard() {
  m_window->hide_keyboard();
}

rocket_file_interface::rocket_file_interface(
    wn::logging::log* _log, wn::file_system::mapping* _mapping)
  : m_log(_log), m_mapping(_mapping) {}

Rml::FileHandle rocket_file_interface::Open(const Rml::String& _path) {
  wn::file_system::result res = wn::file_system::result::ok;
  wn::file_system::file_ptr file =
      m_mapping->open_file(get_string_view(_path), res);
  if (res != wn::file_system::result::ok) {
    return reinterpret_cast<Rml::FileHandle>(nullptr);
  }

  // This makes me sad, eventually fix this
  ui_file* f = new ui_file();
  f->m_offset = 0;
  f->m_file = wn::core::move(file);
  return reinterpret_cast<Rml::FileHandle>(f);
}

void rocket_file_interface::Close(Rml::FileHandle _file) {
  ui_file* f = reinterpret_cast<ui_file*>(_file);
  delete f;
}

size_t rocket_file_interface::Read(
    void* _buffer, size_t _size, Rml::FileHandle _file) {
  ui_file* f = reinterpret_cast<ui_file*>(_file);

  size_t to_read = wn::math::min(_size, f->m_file->size() - f->m_offset);
  wn::memory::memcpy(_buffer, f->m_file->data() + f->m_offset, to_read);
  f->m_offset += to_read;
  return to_read;
}

bool rocket_file_interface::Seek(
    Rml::FileHandle _file, long _offset, int _origin) {
  ui_file* f = reinterpret_cast<ui_file*>(_file);
  WN_RELEASE_ASSERT(_offset >= 0, "_offset must be positive");
  size_t offs = static_cast<size_t>(_offset);
  switch (_origin) {
    case SEEK_SET:
      if (offs > f->m_file->size()) {
        return false;
      }
      f->m_offset = offs;
      break;
    case SEEK_CUR:
      if (offs + f->m_offset > f->m_file->size()) {
        return false;
      }
      f->m_offset += offs;
      break;
    case SEEK_END:
      if (offs > f->m_file->size()) {
        return false;
      }
      f->m_offset = f->m_file->size() - offs;
      break;
  }
  return true;
}

size_t rocket_file_interface::Tell(Rml::FileHandle _file) {
  ui_file* f = reinterpret_cast<ui_file*>(_file);
  return f->m_offset;
}

}  // namespace ui
}  // namespace engine
}  // namespace wn
