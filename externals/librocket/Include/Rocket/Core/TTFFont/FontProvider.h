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

#ifndef ROCKETCORETTFFONTFONTPROVIDER_H
#define ROCKETCORETTFFONTFONTPROVIDER_H

#include "../Context.h"
#include "../Font.h"
#include "../FontProvider.h"
#include "../StringUtilities.h"

namespace Rocket {
namespace Core {

class Context;
class FontEffect;
class FontFaceHandle;
class PropertyDictionary;

namespace TTFFont {
class TTFFontDefinitions;

class FontFamily;

class FontProvider : public Rocket::Core::FontProvider {
public:
  static bool Initialise(Context* _context);
  static void Shutdown(Context* _context);
  static bool LoadFontFace(Context* _context, const String& file_name);
  static bool LoadFontFace(Context* _context, const String& file_name,
      const String& family, Font::Style style, Font::Weight weight);
  static bool LoadFontFace(
      Context* _context, const byte* data, size_t data_length);
  static bool LoadFontFace(Context* _context, const byte* data,
      size_t data_length, const String& family, Font::Style style,
      Font::Weight weight);

  static void RegisterContextValues(Context* context);

private:
  bool AddFace(TTFFontDefinitions* face);
  FontProvider(Context* _context);
  ~FontProvider();

  Context* m_context;
};
}  // namespace TTFFont
}  // namespace Core
}  // namespace Rocket

#endif
