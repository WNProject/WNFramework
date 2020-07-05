// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#include "FontFaceHandle.h"
#include <algorithm>
#include <cmath>
#include "../TextureLayout.h"
#include "../precompiled.h"
#include "FontFaceLayer.h"

#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_rect_pack.h>
#include <stb_truetype.h>

namespace Rocket {
namespace Core {
namespace TTFFont {

class FontEffectSort {
public:
  bool operator()(const Rocket::Core::FontEffect* lhs,
      const Rocket::Core::FontEffect* rhs) {
    return lhs->GetZIndex() < rhs->GetZIndex();
  }
};

FontFaceHandle::FontFaceHandle(Context* _context)
  : Core::FontFaceHandle(_context) {}

FontFaceHandle::~FontFaceHandle() {}

bool FontFaceHandle::Initialise(
    TTFFontDefinitions* _ttf_face, const String& _charset, int _size) {
  m_ttf_face = _ttf_face;
  size = _size;
  raw_charset = _charset;
  if (!UnicodeRange::BuildList(charset, raw_charset)) {
    Log::Message(m_context, Log::LT_ERROR, "Invalid font charset '%s'.",
        raw_charset.CString());
    return false;
  }
  m_scale = stbtt_ScaleForPixelHeight(&_ttf_face->Face.font_info, (float)_size);

  for (size_t i = 0; i < charset.size(); ++i)
    BuildGlyphMap(_ttf_face, charset[i]);

  GenerateMetrics(_ttf_face);

  base_layer = GenerateLayer(NULL);
  layer_configurations.push_back(LayerConfiguration());
  layer_configurations.back().push_back(base_layer);

  return true;
}

int FontFaceHandle::GetStringWidth(
    const WString& string, word prior_character) const {
  float width = 0;

  for (size_t i = 0; i < string.Length(); i++) {
    word character_code = string[i];

    if (character_code >= glyphs.size())
      continue;
    const FontGlyph& glyph = glyphs[character_code];

    if (prior_character != 0)
      width += GetKerning(prior_character, string[i]);
    width += glyph.advance;

    prior_character = character_code;
  }

  return int(::ceil(width));
}

int FontFaceHandle::GenerateLayerConfiguration(FontEffectMap& font_effects) {
  if (font_effects.empty())
    return 0;

  FontEffectList sorted_effects;
  for (FontEffectMap::const_iterator i = font_effects.begin();
       i != font_effects.end(); ++i)
    sorted_effects.push_back(i->second);

  std::sort(sorted_effects.begin(), sorted_effects.end(), FontEffectSort());

  int configuration_index = 1;
  for (; configuration_index < (int)layer_configurations.size();
       ++configuration_index) {
    const LayerConfiguration& configuration =
        layer_configurations[configuration_index];

    if (configuration.size() != sorted_effects.size() + 1)
      continue;

    size_t effect_index = 0;
    for (size_t i = 0; i < configuration.size(); ++i) {
      if (configuration[i]->GetFontEffect() == NULL)
        continue;
      if (configuration[i]->GetFontEffect() != sorted_effects[effect_index])
        break;

      ++effect_index;
    }

    if (effect_index == sorted_effects.size())
      return configuration_index;
  }

  layer_configurations.push_back(LayerConfiguration());
  LayerConfiguration& layer_configuration = layer_configurations.back();

  bool added_base_layer = false;

  for (size_t i = 0; i < sorted_effects.size(); ++i) {
    if (!added_base_layer && sorted_effects[i]->GetZIndex() >= 0) {
      layer_configuration.push_back(base_layer);
      added_base_layer = true;
    }

    layer_configuration.push_back(GenerateLayer(sorted_effects[i]));
  }

  if (!added_base_layer)
    layer_configuration.push_back(base_layer);

  return (int)(layer_configurations.size() - 1);
}

bool FontFaceHandle::GenerateLayerTexture(const byte*& texture_data,
    Vector2i& texture_dimensions, Rocket::Core::FontEffect* layer_id,
    int texture_id) {
  FontLayerMap::iterator layer_iterator = layers.find(layer_id);
  if (layer_iterator == layers.end())
    return false;

  return layer_iterator->second->GenerateTexture(
      texture_data, texture_dimensions, texture_id);
}

int FontFaceHandle::GenerateString(GeometryList& geometry,
    const WString& string, const Vector2f& position, const Colourb& colour,
    int layer_configuration_index) const {
  int geometry_index = 0;
  float line_width = 0;

  ROCKET_ASSERT(layer_configuration_index >= 0);
  ROCKET_ASSERT(layer_configuration_index < (int)layer_configurations.size());

  const LayerConfiguration& layer_configuration =
      layer_configurations[layer_configuration_index];
  for (size_t i = 0; i < layer_configuration.size(); ++i) {
    Rocket::Core::FontFaceLayer* layer = layer_configuration[i];

    Colourb layer_colour;
    if (layer == base_layer)
      layer_colour = colour;
    else
      layer_colour = layer->GetColour();

    size_t extra_geometry = 0;
    if ((int)geometry.size() < geometry_index + layer->GetNumTextures()) {
      extra_geometry =
          geometry_index + layer->GetNumTextures() - geometry.size();
    }

    for (size_t k = 0; k < extra_geometry; ++k) {
      geometry.emplace_back(m_context);
    }

    for (int j = 0; j < layer->GetNumTextures(); ++j)
      geometry[geometry_index + j].SetTexture(layer->GetTexture(j));

    line_width = 0;
    word prior_character = 0;

    const word* string_iterator = string.CString();
    const word* string_end = string.CString() + string.Length();

    for (; string_iterator != string_end; string_iterator++) {
      if (*string_iterator >= glyphs.size())
        continue;
      const FontGlyph& glyph = glyphs[*string_iterator];

      if (prior_character != 0)
        line_width += GetKerning(prior_character, *string_iterator);
      layer->GenerateGeometry(&geometry[geometry_index], *string_iterator,
          Vector2f(position.x + line_width, position.y), layer_colour, 1);

      line_width += glyph.advance;
      prior_character = *string_iterator;
    }

    geometry_index += layer->GetNumTextures();
  }

  // Cull any excess geometry from a previous generation.
  geometry.erase(
      geometry.begin() + geometry_index, geometry.begin() + geometry.size());

  return int(::ceil(line_width));
}

// Generates the geometry required to render a line above, below or through a
// line of text.
void FontFaceHandle::GenerateLine(Geometry* geometry, const Vector2f& position,
    int width, Font::Line height, const Colourb& colour) const {
  std::vector<Vertex>& line_vertices = geometry->GetVertices();
  std::vector<int>& line_indices = geometry->GetIndices();

  float offset;
  switch (height) {
    case Font::UNDERLINE:
      offset = -underline_position;
      break;
    case Font::OVERLINE:        // where to place? offset = -line_height -
                                // underline_position; break;
    case Font::STRIKE_THROUGH:  // where to place? offset = -line_height * 0.5f;
                                // break;
    default:
      return;
  }

  line_vertices.resize(line_vertices.size() + 4);
  line_indices.resize(line_indices.size() + 6);
  GeometryUtilities::GenerateQuad(
      &line_vertices[0] + (line_vertices.size() - 4),
      &line_indices[0] + (line_indices.size() - 6),
      Vector2f(position.x, position.y + offset),
      Vector2f((float)width, underline_thickness), colour,
      static_cast<int>(line_vertices.size() - 4));
}

// Destroys the handle.
void FontFaceHandle::OnReferenceDeactivate() {
  delete this;
}

void FontFaceHandle::GenerateMetrics(TTFFontDefinitions* _tf_face) {
  int ascent;
  int descent;
  int lineGap;
  stbtt_GetFontVMetrics(&_tf_face->Face.font_info, &ascent, &descent, &lineGap);

  float _lineHeight = (float)(ascent - descent + lineGap);
  float _baseline = (float)-descent * m_scale;

  float scale = (float)size / (float)(ascent - descent);
  _lineHeight *= scale;
  line_height = (int)_lineHeight;
  baseline = (int)(_baseline);
  ascent = (int)((float)ascent * scale);
  descent = (int)((float)descent * scale);

  average_advance = 0;
  float av = 0;
  for (FontGlyphList::iterator i = glyphs.begin(); i != glyphs.end(); ++i) {
    av += i->advance;
  }

  // Bring the total advance down to the average advance, but scaled up 10%,
  // just to be on the safe side.
  average_advance = Math::RealToInteger((float)av / (glyphs.size() * 0.9f));

  // Determine the x-height of this font face.
  word x = (word)'x';

  int x0, x1, y0, y1;
  stbtt_GetCodepointBox(&_tf_face->Face.font_info, x, &x0, &y0, &x1, &y1);

  x_height = int((x1 - x0) * scale);
}

void FontFaceHandle::BuildGlyphMap(
    TTFFontDefinitions* _ttf_face, const UnicodeRange& unicode_range) {
  glyphs.resize(unicode_range.max_codepoint + 1);

  for (word character_code =
           (word)(Math::Max<unsigned int>(unicode_range.min_codepoint, 32));
       character_code <= unicode_range.max_codepoint; ++character_code) {
    int index =
        stbtt_FindGlyphIndex(&_ttf_face->Face.font_info, character_code);

    if (index < 0) {
      continue;
    }

    FontGlyph glyph;
    glyph.character = character_code;
    BuildGlyph(_ttf_face, glyph, index);
    glyphs[character_code] = glyph;
  }
}

void Rocket::Core::TTFFont::FontFaceHandle::BuildGlyph(
    TTFFontDefinitions* _ttf_face, FontGlyph& glyph, int index) {
  int x0, x1, y0, y1;
  stbtt_GetGlyphBox(&_ttf_face->Face.font_info, index, &x0, &y0, &x1, &y1);

  int advanceWidth, leftSideBearing;
  stbtt_GetGlyphHMetrics(
      &_ttf_face->Face.font_info, index, &advanceWidth, &leftSideBearing);

  // Set the glyph's dimensions.
  glyph.dimensions.x = (int)((float)(y1 - y0) * m_scale);
  glyph.dimensions.y = (int)((float)(x1 - x0) * m_scale);

  // Set the glyph's advance.
  glyph.advance = (int)((float)advanceWidth * m_scale);

  int ix0, iy0, ix1, iy1;
  stbtt_GetGlyphBitmapBoxSubpixel(&_ttf_face->Face.font_info, index, m_scale,
      m_scale, 0.0f, 0.0f, &ix0, &iy0, &ix1, &iy1);

  // Set the glyph's bearing.
  glyph.bearing.x = (int)((float)leftSideBearing * m_scale);
  glyph.bearing.y = -iy0 + baseline;

  // TODO: Fix up the offset;
  glyph.bitmap_data = stbtt_GetGlyphBitmap(&_ttf_face->Face.font_info, m_scale,
      m_scale, index, &glyph.bitmap_dimensions.x, &glyph.bitmap_dimensions.y,
      nullptr, nullptr);
}

int Rocket::Core::TTFFont::FontFaceHandle::GetKerning(
    word lhs, word rhs) const {
  if (m_ttf_face != nullptr) {
    return (int)(stbtt_GetCodepointKernAdvance(
                     &m_ttf_face->Face.font_info, lhs, rhs) *
                 m_scale);
  }

  return 0;
}

// Generates (or shares) a layer derived from a font effect.
Rocket::Core::FontFaceLayer* FontFaceHandle::GenerateTTFLayer(
    FontEffect* font_effect) {
  // See if this effect has been instanced before, as part of a different
  // configuration.
  FontLayerMap::iterator i = layers.find(font_effect);
  if (i != layers.end())
    return i->second;

  Rocket::Core::TTFFont::FontFaceLayer* layer =
      new Rocket::Core::TTFFont::FontFaceLayer(m_context);
  layers[font_effect] = layer;

  if (font_effect == NULL) {
    layer->Initialise(this);
  } else {
    // Determine which, if any, layer the new layer should copy its geometry and
    // textures from.
    Rocket::Core::FontFaceLayer* clone = NULL;
    bool deep_clone = true;
    String generation_key;

    if (!font_effect->HasUniqueTexture()) {
      clone = base_layer;
      deep_clone = false;
    } else {
      generation_key =
          font_effect->GetName() + ";" + font_effect->GetGenerationKey();
      FontLayerCache::iterator cache_iterator =
          layer_cache.find(generation_key);
      if (cache_iterator != layer_cache.end())
        clone = cache_iterator->second;
    }

    // Create a new layer.
    layer->Initialise(this, font_effect, clone, deep_clone);

    // Cache the layer in the layer cache if it generated its own textures (ie,
    // didn't clone).
    if (clone == NULL)
      layer_cache[generation_key] = (Rocket::Core::FontFaceLayer*)layer;
  }

  return (Rocket::Core::FontFaceLayer*)layer;
}
}  // namespace TTFFont
}  // namespace Core
}  // namespace Rocket
