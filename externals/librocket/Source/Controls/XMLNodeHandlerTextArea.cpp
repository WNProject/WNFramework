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
// found in the LICENSE.txt file.
#include "XMLNodeHandlerTextArea.h"
#include "../../Include/Rocket/Controls/ElementFormControlTextArea.h"
#include "../../Include/Rocket/Core.h"

namespace Rocket {
namespace Controls {

XMLNodeHandlerTextArea::XMLNodeHandlerTextArea(Core::Context* _context)
  : m_context(_context) {}

XMLNodeHandlerTextArea::~XMLNodeHandlerTextArea() {}

Core::Element* XMLNodeHandlerTextArea::ElementStart(Core::XMLParser* parser,
    const Rocket::Core::String& name,
    const Rocket::Core::XMLAttributes& attributes) {
  ElementFormControlTextArea* text_area =
      rocket_dynamic_cast<ElementFormControlTextArea*>(
          parser->GetParseFrame()->element);
  if (text_area == NULL) {
    Core::Element* new_element = Core::Factory::InstanceElement(
        m_context, parser->GetParseFrame()->element, name, name, attributes);
    if (new_element == NULL)
      return NULL;

    parser->GetParseFrame()->element->AppendChild(new_element);
    new_element->RemoveReference();

    return new_element;
  }

  return NULL;
}

bool XMLNodeHandlerTextArea::ElementEnd(
    Core::XMLParser* ROCKET_UNUSED_PARAMETER(parser),
    const Rocket::Core::String& ROCKET_UNUSED_PARAMETER(name)) {
  return true;
}

bool XMLNodeHandlerTextArea::ElementData(
    Core::XMLParser* parser, const Rocket::Core::String& data) {
  ElementFormControlTextArea* text_area =
      rocket_dynamic_cast<ElementFormControlTextArea*>(
          parser->GetParseFrame()->element);
  if (text_area != NULL) {
    // Do any necessary translation.
    Rocket::Core::String translated_data;
    Core::GetSystemInterface(m_context)->TranslateString(translated_data, data);

    text_area->SetValue(translated_data);
  }

  return true;
}

void XMLNodeHandlerTextArea::Release() {
  delete this;
}
}  // namespace Controls
}  // namespace Rocket
