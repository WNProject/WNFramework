// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNScripting/inc/WNFactory.h"
#include "WNMultiTasking/inc/thread.h"


using namespace wn;

namespace wn {
namespace engine {

namespace {
void sleep(int32_t _seconds){
  multi_tasking::this_thread::sleep_for(std::chrono::seconds(_seconds));
};
}  // anonymous namespace

void register_scripting(scripting::engine* _engine) {
  _engine->register_function<decltype(&sleep), &sleep>("sleep");
}

}  // namespace engine
}  // namespace wn