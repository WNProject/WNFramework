// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
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
