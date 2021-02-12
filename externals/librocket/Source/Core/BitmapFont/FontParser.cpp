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
#include "FontParser.h"
#include "../precompiled.h"

namespace Rocket {
namespace Core {
namespace BitmapFont {

FontParser::FontParser(Context* _context, BitmapFontDefinitions* face)
  : BaseXMLParser(_context) {
  bm_face = face;
  char_id = 0;
  kern_id = 0;
}

FontParser::~FontParser() {}

// Called when the parser finds the beginning of an element tag.
void FontParser::HandleElementStart(
    const String& name, const XMLAttributes& _attributes) {
  if (name == "info") {
    bm_face->Face.FamilyName = _attributes.Get("face")->Get<String>();
    bm_face->Face.Size = _attributes.Get("size")->Get<int>();
    bm_face->Face.Weight = _attributes.Get("bold")->Get<bool>()
                               ? Font::WEIGHT_BOLD
                               : Font::WEIGHT_NORMAL;
    bm_face->Face.Style = _attributes.Get("italic")->Get<bool>()
                              ? Font::STYLE_ITALIC
                              : Font::STYLE_NORMAL;

  } else if (name == "page") {
    // TODO: Handle multiple pages
    bm_face->Face.BitmapSource = _attributes.Get("file")->Get<String>();
  } else if (name == "common") {
    bm_face->CommonCharactersInfo.LineHeight =
        _attributes.Get("lineHeight")->Get<int>();
    bm_face->CommonCharactersInfo.BaseLine =
        _attributes.Get("base")->Get<int>() * -1;
    bm_face->CommonCharactersInfo.ScaleWidth =
        _attributes.Get("scaleW")->Get<int>();
    bm_face->CommonCharactersInfo.ScaleHeight =
        _attributes.Get("scaleH")->Get<int>();
    bm_face->CommonCharactersInfo.CharacterCount = 0;
    bm_face->CommonCharactersInfo.KerningCount = 0;
  } else if (name == "chars") {
    bm_face->CommonCharactersInfo.CharacterCount =
        _attributes.Get("count")->Get<int>();
    bm_face->CharactersInfo =
        new CharacterInfo[_attributes.Get("count")->Get<int>()];
  } else if (name == "char") {
    bm_face->CharactersInfo[char_id].Id = _attributes.Get("id")->Get<int>();
    bm_face->CharactersInfo[char_id].X =
        _attributes.Get("x")->Get<int>();  // The left position of the character
                                           // image in the texture.
    bm_face->CharactersInfo[char_id].Y =
        _attributes.Get("y")->Get<int>();  // The top position of the character
                                           // image in the texture.
    bm_face->CharactersInfo[char_id].Width =
        _attributes.Get("width")
            ->Get<int>();  // The width of the character image in the texture.
    bm_face->CharactersInfo[char_id].Height =
        _attributes.Get("height")
            ->Get<int>();  // The height of the character image in the texture.
    bm_face->CharactersInfo[char_id].XOffset =
        _attributes.Get("xoffset")->Get<int>();
    bm_face->CharactersInfo[char_id].YOffset =
        _attributes.Get("yoffset")->Get<int>();
    bm_face->CharactersInfo[char_id].Advance =
        _attributes.Get("xadvance")->Get<int>();

    char_id++;
  } else if (name == "kernings") {
    bm_face->CommonCharactersInfo.KerningCount =
        _attributes.Get("count")->Get<int>();
    bm_face->KerningsInfo =
        new KerningInfo[_attributes.Get("count")->Get<int>()];
  } else if (name == "kerning") {
    bm_face->KerningsInfo[kern_id].FirstCharacterId =
        _attributes.Get("first")->Get<int>();
    bm_face->KerningsInfo[kern_id].SecondCharacterId =
        _attributes.Get("second")->Get<int>();
    bm_face->KerningsInfo[kern_id].KerningAmount =
        _attributes.Get("amount")->Get<int>();

    kern_id++;
  }
}

// Called when the parser finds the end of an element tag.
void FontParser::HandleElementEnd(const String& ROCKET_UNUSED_PARAMETER(name)) {
}

// Called when the parser encounters data.
void FontParser::HandleData(const String& ROCKET_UNUSED_PARAMETER(data)) {}
}  // namespace BitmapFont
}  // namespace Core
}  // namespace Rocket
