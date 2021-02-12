// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SUPPORT_JSON_H_
#define __WN_SUPPORT_JSON_H_

#include "WNScripting/inc/WNEngine.h"
#include "rapidjson/document.h"

namespace wn {
namespace support {
struct json_value;

class json_document {
public:
  json_document(rapidjson::Document _doc) : m_document(core::move(_doc)) {}
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);
  json_value* value();

private:
  rapidjson::Document m_document;
};

}  // namespace support
}  // namespace wn

#endif  // __WN_SUPPORT_JSON_H_
