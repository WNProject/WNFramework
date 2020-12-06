// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNMultiTasking/inc/thread.h"
#include "WNScripting/inc/WNFactory.h"

using namespace wn;

namespace wn {
namespace engine {

namespace {
void sleep(int32_t _seconds) {
  multi_tasking::this_thread::sleep_for(std::chrono::seconds(_seconds));
};

void log_error(const char* _str) {
  scripting::g_scripting_tls->_log->log_error(_str);
}

}  // anonymous namespace

void register_scripting(scripting::engine* _engine) {
  _engine->register_function<decltype(&sleep), &sleep>("sleep");
  _engine->register_function<decltype(&log_error), &log_error>("log_error");
}

}  // namespace engine
}  // namespace wn
