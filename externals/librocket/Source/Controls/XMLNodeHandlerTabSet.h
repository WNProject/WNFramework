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
// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef ROCKETCONTROLSXMLNODEHANDLERTABSET_H
#define ROCKETCONTROLSXMLNODEHANDLERTABSET_H

#include "../../Include/Rocket/Core/XMLNodeHandler.h"

namespace Rocket {
namespace Core {
class Context;
}
namespace Controls {

/**
        XML node handler for processing the tabset tags.

        @author Lloyd Weehuizen
 */

class XMLNodeHandlerTabSet : public Core::XMLNodeHandler {
public:
  XMLNodeHandlerTabSet(Core::Context* _context);
  virtual ~XMLNodeHandlerTabSet();

  /// Called when a new element start is opened
  virtual Core::Element* ElementStart(Core::XMLParser* parser,
      const Rocket::Core::String& name,
      const Rocket::Core::XMLAttributes& attributes);
  /// Called when an element is closed
  virtual bool ElementEnd(
      Core::XMLParser* parser, const Rocket::Core::String& name);
  /// Called for element data
  virtual bool ElementData(
      Core::XMLParser* parser, const Rocket::Core::String& data);

  virtual void Release();

  Core::Context* m_context;
};
}  // namespace Controls
}  // namespace Rocket

#endif
