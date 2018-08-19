/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#include "WidgetTextInputSingleLinePassword.h"
#include "../../Include/Rocket/Core/ElementText.h"

namespace Rocket {
namespace Controls {

WidgetTextInputSingleLinePassword::WidgetTextInputSingleLinePassword(
    Core::Context* _context, ElementFormControl* parent)
  : WidgetTextInputSingleLine(_context, parent) {}

WidgetTextInputSingleLinePassword::~WidgetTextInputSingleLinePassword() {}

// Sets the value of the password field.
void WidgetTextInputSingleLinePassword::SetValue(const Core::String& value) {
  Core::String sanitised_value(value);
  SanitiseValue(sanitised_value);
  Core::WString wString_sanitised_value(sanitised_value);
  WidgetTextInput::SetValue(
      Core::String(wString_sanitised_value.Length(), (Rocket::Core::word)'*'));
}
}  // namespace Controls
}  // namespace Rocket