// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_UI_ROCKET_INTEROP_H__
#define __WN_ENGINE_UI_ROCKET_INTEROP_H__

#include <chrono>
#include "RmlUi/Core.h"
#include "WNLogging/inc/WNLog.h"
namespace wn {
namespace file_system {
class mapping;
}
namespace runtime {
namespace window {
class window;
}
}  // namespace runtime

namespace engine {
namespace ui {

class rocket_system_interface : public Rml::SystemInterface {
public:
  rocket_system_interface(
      wn::logging::log* _log, wn::runtime::window::window* _window);

  bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;

  double GetElapsedTime() override;

  // Activate keyboard (for touchscreen devices)
  void ActivateKeyboard(
      Rml::Vector2f caret_position, float line_height) override;

  // Deactivate keyboard (for touchscreen devices)
  void DeactivateKeyboard() override;

private:
  wn::logging::log* m_log;
  std::chrono::high_resolution_clock::time_point m_start;
  wn::runtime::window::window* m_window;
};

class rocket_file_interface : public Rml::FileInterface {
public:
  rocket_file_interface(
      wn::logging::log* _log, wn::file_system::mapping* _mapping);

  Rml::FileHandle Open(const Rml::String& _path) override;
  void Close(Rml::FileHandle _file) override;
  size_t Read(void* _buffer, size_t _size, Rml::FileHandle _file) override;

  bool Seek(Rml::FileHandle _file, long _offset, int _origin) override;
  size_t Tell(Rml::FileHandle _file) override;

private:
  wn::logging::log* m_log;
  wn::file_system::mapping* m_mapping;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif  // __WN_ENGINE_UI_ROCKET_INTEROP_H__
