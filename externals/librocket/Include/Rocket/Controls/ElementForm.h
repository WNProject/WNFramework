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
#ifndef ROCKETCONTROLSELEMENTFORM_H
#define ROCKETCONTROLSELEMENTFORM_H

#include "../Core/Element.h"

namespace Rocket {
namespace Core {
class Context;
}
namespace Controls {

/**
        A specialisation of the generic Core::Element representing a form
   element.

        @author Peter Curry
 */

class ElementForm : public Core::Element {
public:
  ROCKET_RTTI_DefineWithParent(Core::Element);
  /// Constructs a new ElementForm. This should not be called directly; use the
  /// Factory instead.
  /// @param[in] tag The tag the element was declared as in RML.
  ElementForm(Core::Context* _context, const Rocket::Core::String& tag);
  virtual ~ElementForm();

  /// Submits the form.
  /// @param[in] name The name of the item doing the submit
  /// @param[in] submit_value The value to send through as the 'submit'
  /// parameter.
  void Submit(const Rocket::Core::String& name = "",
      const Rocket::Core::String& submit_value = "");
};
}  // namespace Controls
}  // namespace Rocket

#endif
