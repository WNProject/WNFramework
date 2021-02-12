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
#ifndef ROCKETCOREBITMAPFONTFONTPROVIDER_H
#define ROCKETCOREBITMAPFONTFONTPROVIDER_H

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

namespace BitmapFont {

class FontFamily;

/**
    The font database contains all font families currently in use by Rocket.
    @author Peter Curry
 */

class FontProvider : public Rocket::Core::FontProvider {
public:
  static bool Initialise(Context* _context);
  static void Shutdown(Context* _context);

  /// Adds a new font face to the database. The face's family, style and weight
  /// will be determined from the face itself.
  /// @param[in] file_name The file to load the face from.
  /// @return True if the face was loaded successfully, false otherwise.
  static bool LoadFontFace(Context* _context, const String& file_name);
  /// Adds a new font face to the database, ignoring any family, style and
  /// weight information stored in the face itself.
  /// @param[in] file_name The file to load the face from.
  /// @param[in] family The family to add the face to.
  /// @param[in] style The style of the face (normal or italic).
  /// @param[in] weight The weight of the face (normal or bold).
  /// @return True if the face was loaded successfully, false otherwise.
  static bool LoadFontFace(Context* _context, const String& file_name,
      const String& family, Font::Style style, Font::Weight weight);
  /// Adds a new font face to the database, loading from memory. The face's
  /// family, style and weight will be determined from the face itself.
  /// @param[in] data The font data.
  /// @param[in] data_length Length of the data.
  /// @return True if the face was loaded successfully, false otherwise.
  static bool LoadFontFace(
      Context* _context, const byte* data, int data_length);
  /// Adds a new font face to the database, loading from memory.
  /// @param[in] data The font data.
  /// @param[in] data_length Length of the data.
  /// @param[in] family The family to add the face to.
  /// @param[in] style The style of the face (normal or italic).
  /// @param[in] weight The weight of the face (normal or bold).
  /// @return True if the face was loaded successfully, false otherwise.
  static bool LoadFontFace(Context* _context, const byte* data, int data_length,
      const String& family, Font::Style style, Font::Weight weight);

  static void RegisterContextValues(Context* context);

private:
  FontProvider(Context* _context);
  ~FontProvider();

  // Adds a loaded face to the appropriate font family.
  bool AddFace(void* face, const String& family, Font::Style style,
      Font::Weight weight, bool release_stream);

  void* LoadFace(const String& file_name);

  void* LoadFace(
      const byte* data, int data_length, const String& source, bool local_data);
  Context* m_context;
};
}  // namespace BitmapFont
}  // namespace Core
}  // namespace Rocket

#endif
