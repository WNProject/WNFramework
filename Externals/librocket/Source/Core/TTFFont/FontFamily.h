// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#ifndef ROCKETCORETTFFONTFAMILY_H
#define ROCKETCORETTFFONTFAMILY_H

#include "../../../Include/Rocket/Core/Font.h"
#include "../../../Include/Rocket/Core/FontFamily.h"
#include "TTFFontDefinitions.h"

namespace Rocket {
namespace Core {
class Context;
class FontFace;
class FontFaceHandle;

namespace TTFFont {

class FontFamily : public Rocket::Core::FontFamily {
public:
  FontFamily(Context* _context, const String& name);
  ~FontFamily();
  bool AddFace(void* ttf_face, Font::Style style, Font::Weight weight,
      bool release_stream);
};
}  // namespace TTFFont
}  // namespace Core
}  // namespace Rocket

#endif
