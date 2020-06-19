// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#include "FontFaceLayer.h"
#include "../precompiled.h"
#include "FontFaceHandle.h"

namespace Rocket {
namespace Core {
namespace TTFFont {

FontFaceLayer::FontFaceLayer(Context* _context)
  : Rocket::Core::FontFaceLayer(_context) {
  handle = NULL;
  effect = NULL;
}

FontFaceLayer::~FontFaceLayer() {}

bool FontFaceLayer::Initialise(const Rocket::Core::FontFaceHandle* _handle,
    FontEffect* _effect, const Rocket::Core::FontFaceLayer* clone,
    bool deep_clone) {
  (void)(_effect);
  (void)(deep_clone);

  handle = _handle;

  const FontGlyphList& glyphs = handle->GetGlyphs();

  if (clone != NULL) {
    characters = clone->characters;
  } else {
    characters.resize(glyphs.size(), Character());
    for (FontGlyphList::const_iterator i = glyphs.begin(); i != glyphs.end();
         ++i) {
      const FontGlyph& glyph = *i;

      if (glyph.dimensions.x <= 0 || glyph.dimensions.y <= 0)
        continue;

      Vector2i glyph_origin(
          glyph.bitmap_dimensions.x, glyph.bitmap_dimensions.y);
      Vector2i glyph_dimensions = glyph.dimensions;

      Character character;
      character.origin = Vector2f((float)(glyph.bearing.x),
          (float)(glyph.bearing.y) - handle->GetBaseline() * 3);
      character.dimensions =
          Vector2f((float)glyph.dimensions.x, (float)glyph.dimensions.y);

      character.texture_index = 0;

      character.texcoords[0].x = (float)glyph_origin.x;
      character.texcoords[0].y = (float)glyph_origin.y;
      character.texcoords[1].x = float(glyph_origin.x + character.dimensions.x);
      character.texcoords[1].y = float(glyph_origin.y + character.dimensions.y);
      characters[glyph.character] = character;

      texture_layout.AddRectangle(glyph.character, glyph_dimensions);
    }

    if (!texture_layout.GenerateLayout(512))
      return false;
  }
  return true;
}
bool FontFaceLayer::GenerateTexture(
    const byte*& texture_data, Vector2i& texture_dimensions, int texture_id) {
  if (texture_id < 0 || texture_id > texture_layout.GetNumTextures()) {
    return false;
  }

  Rocket::Core::TTFFont::FontFaceHandle* ttf_font_face_handle;
  ttf_font_face_handle = (Rocket::Core::TTFFont::FontFaceHandle*)handle;

  const FontGlyphList& glyphs = handle->GetGlyphs();

  texture_data = texture_layout.GetTexture(texture_id).AllocateTexture();
  texture_dimensions = texture_layout.GetTexture(texture_id).GetDimensions();

  for (int i = 0; i < texture_layout.GetNumRectangles(); ++i) {
    TextureLayoutRectangle& rectangle = texture_layout.GetRectangle(i);
    Character& character = characters[(word)rectangle.GetId()];

    if (character.texture_index != texture_id)
      continue;

    const FontGlyph& glyph = glyphs[rectangle.GetId()];

    if (effect == NULL) {
      //
      byte* destination = rectangle.GetTextureData();
      stbtt_MakeCodepointBitmap(&ttf_font_face_handle->GetTTF()->Face.font_info,
          (unsigned char*)destination, glyph.dimensions.x, glyph.dimensions.y,
          rectangle.GetTextureStride(),
          ttf_font_face_handle->GetTTF()->Face.scale,
          ttf_font_face_handle->GetTTF()->Face.scale, rectangle.GetId());
    } else {
      int width, height, xoff, yoff;

      unsigned char* bm = stbtt_GetCodepointBitmap(
          &ttf_font_face_handle->GetTTF()->Face.font_info,
          ttf_font_face_handle->GetTTF()->Face.scale,
          ttf_font_face_handle->GetTTF()->Face.scale, rectangle.GetId(), &width,
          &height, &xoff, &yoff);

      effect->GenerateGlyphTexture(rectangle.GetTextureData(),
          Vector2i(Math::RealToInteger(character.dimensions.x),
              Math::RealToInteger(character.dimensions.y)),
          rectangle.GetTextureStride(), glyph);
      free(bm);
    }
  }
  return true;
}

}  // namespace TTFFont
}  // namespace Core
}  // namespace Rocket
