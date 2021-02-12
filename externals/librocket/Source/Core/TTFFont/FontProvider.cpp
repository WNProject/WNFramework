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
// found in the LICENSE file.
#include <Rocket/Core.h>
#include <Rocket/Core/FontDatabase.h>
#include <Rocket/Core/StreamMemory.h>
#include <Rocket/Core/TTFFont/FontProvider.h>
#include "../FontFaceHandle.h"
#include "../precompiled.h"
#include "FontFamily.h"
#include "TTFFontDefinitions.h"

#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_rect_pack.h>
#include <stb_truetype.h>

namespace Rocket {
namespace Core {
namespace TTFFont {

static FontProvider** kFontProviderKey = 0;

void FontProvider::RegisterContextValues(Context* _context) {
  _context->RegisterCachedType(kFontProviderKey, nullptr);
}

FontProvider::FontProvider(Context* _context) : m_context(_context) {
  auto& instance = (*_context)(kFontProviderKey);
  ROCKET_ASSERT(instance == NULL);
  instance = this;
}

FontProvider::~FontProvider() {
  auto& instance = (*m_context)(kFontProviderKey);
  ROCKET_ASSERT(instance == this);
  instance = NULL;
}

bool FontProvider::Initialise(Context* _context) {
  auto& instance = (*_context)(kFontProviderKey);
  if (instance == NULL) {
    new FontProvider(_context);
    FontDatabase::AddFontProvider(_context, instance);
  }
  return true;
}

void FontProvider::Shutdown(Context* _context) {
  auto& instance = (*_context)(kFontProviderKey);
  if (instance != NULL) {
    FontDatabase::RemoveFontProvider(_context, instance);
    delete instance;
    instance = NULL;
  }
}

bool FontProvider::LoadFontFace(Context* _context, const String& file_name) {
  FileInterface* file_interface = GetFileInterface(_context);
  FileHandle handle = file_interface->Open(file_name);

  if (!handle) {
    return false;
  }

  size_t length = file_interface->Length(handle);

  byte* buffer = new byte[length];
  file_interface->Read(buffer, length, handle);
  file_interface->Close(handle);

  return LoadFontFace(_context, buffer, length);
}

bool FontProvider::LoadFontFace(Context* _context, const String& file_name,
    const String& family, Font::Style style, Font::Weight weight) {
  FileInterface* file_interface = GetFileInterface(_context);
  FileHandle handle = file_interface->Open(file_name);

  if (!handle) {
    return false;
  }

  size_t length = file_interface->Length(handle);

  byte* buffer = new byte[length];
  file_interface->Read(buffer, length, handle);
  file_interface->Close(handle);

  return LoadFontFace(_context, buffer, length, family, style, weight);
}

bool FontProvider::LoadFontFace(Context* _context, const byte* data,
    size_t ROCKET_UNUSED_PARAMETER(data_length)) {
  TTFFontDefinitions* ttf_face = new TTFFontDefinitions();
  if (!stbtt_InitFont(&ttf_face->Face.font_info, data, 0)) {
    return false;
  }

  int length = 0;
  // For now assume MAC, ROMAN, element 1
  // WE can expand this in the future if it is not correct
  // This should be the family name.
  const char* name = stbtt_GetFontNameString(&ttf_face->Face.font_info, &length,
      STBTT_PLATFORM_ID_MAC, STBTT_MAC_EID_ROMAN, 0, 1);
  ROCKET_ASSERT(name);

  ttf_face->Face.Style = Font::STYLE_NORMAL;
  ttf_face->Face.Weight = Font::WEIGHT_NORMAL;
  ttf_face->Face.FamilyName = String(name, name + length);

  auto& instance = (*_context)(kFontProviderKey);
  if (instance->AddFace(ttf_face)) {
    Log::Message(_context, Log::LT_INFO, "Loaded font face %s.",
        ttf_face->Face.FamilyName.CString());
    return true;
  }
  delete[] ttf_face->Face.font_info.data;
  Log::Message(_context, Log::LT_ERROR, "Failed to load font face %s.",
      ttf_face->Face.FamilyName.CString());
  return true;
}

bool FontProvider::LoadFontFace(Context* _context, const byte* data,
    size_t ROCKET_UNUSED_PARAMETER(data_length), const String& family,
    Font::Style style, Font::Weight weight) {
  int fs = 0;
  if (style == Font::STYLE_ITALIC) {
    fs |= STBTT_MACSTYLE_ITALIC;
  }
  if (weight == Font::WEIGHT_BOLD) {
    fs |= STBTT_MACSTYLE_BOLD;
  }

  int offs = stbtt_FindMatchingFont(data, family.CString(), fs);
  if (offs < 0) {
    offs = 0;
  }
  TTFFontDefinitions* ttf_face = new TTFFontDefinitions();
  if (!stbtt_InitFont(&ttf_face->Face.font_info, data, offs)) {
    return false;
  }
  ttf_face->Face.Style = style;
  ttf_face->Face.Weight = weight;
  ttf_face->Face.FamilyName = family;

  auto& instance = (*_context)(kFontProviderKey);
  if (instance->AddFace(ttf_face)) {
    Log::Message(_context, Log::LT_INFO, "Loaded font face %s.",
        ttf_face->Face.FamilyName.CString());
    return true;
  }
  delete[] ttf_face->Face.font_info.data;
  Log::Message(_context, Log::LT_ERROR, "Font face already loaded %s.",
      ttf_face->Face.FamilyName.CString());
  return true;
}

bool FontProvider::AddFace(TTFFontDefinitions* face) {
  auto it = font_families.find(face->Face.FamilyName);
  if (it == font_families.end()) {
    std::tie(it, std::ignore) =
        font_families.insert(std::make_pair(face->Face.FamilyName,
            new FontFamily(m_context, face->Face.FamilyName)));
  }
  return it->second->AddFace(face, face->Face.Style, face->Face.Weight, false);
}

}  // namespace TTFFont
}  // namespace Core
}  // namespace Rocket
