// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#include "FontFamily.h"
#include "../precompiled.h"
#include "FontFace.h"

namespace Rocket {
namespace Core {
namespace TTFFont {

FontFamily::FontFamily(Context* _context, const String& name)
  : Rocket::Core::FontFamily(_context, name) {}

FontFamily::~FontFamily() {}

// Adds a new face to the family.
bool FontFamily::AddFace(
    void* ttf_face, Font::Style style, Font::Weight weight, bool) {
  for (size_t i = 0; i < font_faces.size(); ++i) {
    auto& f = font_faces[i];
    if (f->GetStyle() == style && f->GetWeight() == weight)
      return false;
  }
  Rocket::Core::FontFace* face =
      new FontFace(m_context, static_cast<TTFFontDefinitions*>(ttf_face));
  font_faces.push_back(face);

  return true;
}
}  // namespace TTFFont
}  // namespace Core
}  // namespace Rocket
