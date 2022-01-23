// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef ROCKETCORETTFFONTFACELAYER_H
#define ROCKETCORETTFFONTFACELAYER_H

#include <Rocket/Core/FontGlyph.h>
#include <Rocket/Core/Geometry.h>
#include <Rocket/Core/GeometryUtilities.h>
#include <Rocket/Core/String.h>
#include "../FontFaceLayer.h"

namespace Rocket {
namespace Core {

class Context;
class TextureLayout;

namespace TTFFont {
class FontFaceLayer : public Rocket::Core::FontFaceLayer {
public:
  FontFaceLayer(Context* _context);
  virtual ~FontFaceLayer();
  virtual bool Initialise(const FontFaceHandle* handle,
      FontEffect* effect = NULL,
      const Rocket::Core::FontFaceLayer* clone = NULL, bool deep_clone = false);
  virtual bool GenerateTexture(
      const byte*& texture_data, Vector2i& texture_dimensions, int texture_id);
};
}  // namespace TTFFont
}  // namespace Core
}  // namespace Rocket

#endif
