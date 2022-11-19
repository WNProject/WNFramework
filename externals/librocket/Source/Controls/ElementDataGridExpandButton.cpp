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

// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// clang-format off

#include "../../Include/Rocket/Controls/ElementDataGridExpandButton.h"
#include "../../Include/Rocket/Controls/ElementDataGridRow.h"

namespace Rocket {
namespace Controls {

ElementDataGridExpandButton::ElementDataGridExpandButton(
    Core::Context* _context, const Rocket::Core::String& tag)
  : Core::Element(_context, tag) {
  SetClass("collapsed", true);
}

ElementDataGridExpandButton::~ElementDataGridExpandButton() {}

void ElementDataGridExpandButton::ProcessEvent(Core::Event& event) {
  Core::Element::ProcessEvent(event);

  if (event == "click" && event.GetCurrentElement() == this) {
    // Look for the first data grid row above us, and toggle their on/off
    // state.
    Core::Element* p = GetParentNode();
    ElementDataGridRow* parent_row;
    do {
      parent_row = rocket_dynamic_cast<ElementDataGridRow*>(p);
      p = p->GetParentNode();
    } while (p && !parent_row);

    if (parent_row) {
      parent_row->ToggleRow();

      if (parent_row->IsRowExpanded()) {
        SetClass("collapsed", false);
        SetClass("expanded", true);
      } else {
        SetClass("collapsed", true);
        SetClass("expanded", false);
      }
    }
  }
}
}  // namespace Controls
}  // namespace Rocket
