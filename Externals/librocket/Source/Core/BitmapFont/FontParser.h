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
#ifndef BITMAPFONTPARSER_H
#define BITMAPFONTPARSER_H

#include <Rocket/Core/Dictionary.h>
#include <Rocket/Core/Types.h>
#include <Rocket/Core/XMLParser.h>
#include <set>
#include "BitmapFontDefinitions.h"

namespace Rocket {
namespace Core {
class Context;
namespace BitmapFont {

/**
        @author Peter Curry
 */

class FontParser : public BaseXMLParser {
public:
  FontParser(Context* _context, BitmapFontDefinitions* face);
  virtual ~FontParser();

  /// Called when the parser finds the beginning of an element tag.
  virtual void HandleElementStart(
      const String& name, const XMLAttributes& attributes);
  /// Called when the parser finds the end of an element tag.
  virtual void HandleElementEnd(const String& name);
  /// Called when the parser encounters data.
  virtual void HandleData(const String& data);

private:
  FontParser();
  BitmapFontDefinitions* bm_face;
  int char_id;
  int kern_id;
};
}  // namespace BitmapFont
}  // namespace Core
}  // namespace Rocket
#endif
