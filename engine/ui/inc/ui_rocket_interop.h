// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_UI_ROCKET_INTEROP_H__
#define __WN_ENGINE_UI_ROCKET_INTEROP_H__

#include <time.h>
#include "Rocket/Core.h"
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

class rocket_system_interface : public Rocket::Core::SystemInterface {
public:
  rocket_system_interface(
      wn::logging::log* _log, wn::runtime::window::window* _window);

  bool LogMessage(Rocket::Core::Log::Type type,
      const Rocket::Core::String& message) override;

  float GetElapsedTime() override;

  // Activate keyboard (for touchscreen devices)
  void ActivateKeyboard() override;

  // Deactivate keyboard (for touchscreen devices)
  void DeactivateKeyboard() override;

private:
  wn::logging::log* m_log;
  time_t m_start;
  wn::runtime::window::window* m_window;
};

class rocket_file_interface : public Rocket::Core::FileInterface {
public:
  rocket_file_interface(
      wn::logging::log* _log, wn::file_system::mapping* _mapping);

  Rocket::Core::FileHandle Open(const Rocket::Core::String& _path) override;
  void Close(Rocket::Core::FileHandle _file) override;
  size_t Read(
      void* _buffer, size_t _size, Rocket::Core::FileHandle _file) override;

  bool Seek(Rocket::Core::FileHandle _file, long _offset, int _origin) override;
  size_t Tell(Rocket::Core::FileHandle _file) override;

private:
  wn::logging::log* m_log;
  wn::file_system::mapping* m_mapping;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif  // __WN_ENGINE_UI_ROCKET_INTEROP_H__
