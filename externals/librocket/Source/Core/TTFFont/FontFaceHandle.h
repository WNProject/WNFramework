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

#ifndef ROCKETCORETTFFONTFACEHANDLE_H
#define ROCKETCORETTFFONTFACEHANDLE_H

#include "../../../Include/Rocket/Core/Font.h"
#include "../../../Include/Rocket/Core/FontEffect.h"
#include "../../../Include/Rocket/Core/FontGlyph.h"
#include "../../../Include/Rocket/Core/Geometry.h"
#include "../../../Include/Rocket/Core/String.h"
#include "../../../Include/Rocket/Core/Texture.h"
#include "../FontFaceHandle.h"
#include "../UnicodeRange.h"
#include "TTFFontDefinitions.h"

namespace Rocket {
namespace Core {
class Context;
namespace TTFFont {
class FontFaceLayer;
class FontFaceHandle : public Rocket::Core::FontFaceHandle {
public:
  FontFaceHandle(Context* _context);
  virtual ~FontFaceHandle();

  bool Initialise(
      TTFFontDefinitions* ttf_Face, const String& charset, int size);
  int GetStringWidth(const WString& string, word prior_character = 0) const;
  int GenerateLayerConfiguration(Rocket::Core::FontEffectMap& font_effects);
  bool GenerateLayerTexture(const byte*& texture_data,
      Vector2i& texture_dimensions, FontEffect* layer_id, int texture_id);
  int GenerateString(GeometryList& geometry, const WString& string,
      const Vector2f& position, const Colourb& colour,
      int layer_configuration = 0) const;
  void GenerateLine(Geometry* geometry, const Vector2f& position, int width,
      Font::Line height, const Colourb& colour) const;
  TTFFontDefinitions* GetTTF() const {
    return m_ttf_face;
  }

protected:
  /// Destroys the handle.
  virtual void OnReferenceDeactivate();

private:
  Rocket::Core::FontFaceLayer* GenerateTTFLayer(FontEffect* font_effect);

  void GenerateMetrics(TTFFontDefinitions* bm_face);

  void BuildGlyphMap(
      TTFFontDefinitions* ttf_face, const UnicodeRange& unicode_range);
  void BuildGlyph(
      TTFFontDefinitions* ttf_face, FontGlyph& glyph, int glyph_index);
  int GetKerning(word lhs, word rhs) const;

  TTFFontDefinitions* m_ttf_face;
  float m_scale = 0;
};
}  // namespace TTFFont
}  // namespace Core
}  // namespace Rocket

#endif
