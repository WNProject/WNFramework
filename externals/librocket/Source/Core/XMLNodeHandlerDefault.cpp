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
#include "XMLNodeHandlerDefault.h"
#include "../../Include/Rocket/Core/Element.h"
#include "../../Include/Rocket/Core/Factory.h"
#include "../../Include/Rocket/Core/Log.h"
#include "../../Include/Rocket/Core/XMLParser.h"
#include "XMLParseTools.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

XMLNodeHandlerDefault::XMLNodeHandlerDefault(Context* _context)
  : m_context(_context) {}

XMLNodeHandlerDefault::~XMLNodeHandlerDefault() {}

Element* XMLNodeHandlerDefault::ElementStart(
    XMLParser* parser, const String& name, const XMLAttributes& attributes) {
  // Determine the parent
  Element* parent = parser->GetParseFrame()->element;

  // Attempt to instance the element with the instancer
  Element* element =
      Factory::InstanceElement(m_context, parent, name, name, attributes);
  if (!element) {
    Log::Message(m_context, Log::LT_ERROR,
        "Failed to create element for tag %s, instancer returned NULL.",
        name.CString());
    return NULL;
  }

  // Add the element to its parent and remove the reference
  parent->AppendChild(element);
  element->RemoveReference();

  return element;
}

bool XMLNodeHandlerDefault::ElementEnd(
    XMLParser* ROCKET_UNUSED_PARAMETER(parser),
    const String& ROCKET_UNUSED_PARAMETER(name)) {
  return true;
}

bool XMLNodeHandlerDefault::ElementData(XMLParser* parser, const String& data) {
  // Determine the parent
  Element* parent = parser->GetParseFrame()->element;

  // Parse the text into the element
  return Factory::InstanceElementText(m_context, parent, data);
}

void XMLNodeHandlerDefault::Release() {
  delete this;
}
}  // namespace Core
}  // namespace Rocket
