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
#include "../../Include/Rocket/Core/FontFamily.h"
#include "../../Include/Rocket/Core/FontFace.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

FontFamily::FontFamily(Context* _context, const String& name)
  : name(name), m_context(_context) {}

FontFamily::~FontFamily() {
  for (size_t i = 0; i < font_faces.size(); ++i)
    delete font_faces[i];
}

// Returns a handle to the most appropriate font in the family, at the correct
// size.
FontFaceHandle* FontFamily::GetFaceHandle(
    const String& charset, Font::Style style, Font::Weight weight, int size) {
  // Search for a face of the same style, and match the weight as closely as we
  // can.
  FontFace* matching_face = NULL;
  uint32_t max_difference = static_cast<uint32_t>(-1);
  for (size_t i = 0; i < font_faces.size(); i++) {
    // If we've found a face matching the style, then ... great! We'll match it
    // regardless of the weight. However,
    // if it's a perfect match, then we'll stop looking altogether.
    if (font_faces[i]->GetStyle() == style) {
      if (font_faces[i]->GetFaceSize() != -1) {
        const uint32_t face_size =
            static_cast<uint32_t>(font_faces[i]->GetFaceSize());
        const uint32_t diff = face_size > static_cast<uint32_t>(size)
                                  ? face_size - static_cast<uint32_t>(size)
                                  : static_cast<uint32_t>(size) - face_size;
        if (diff == 0) {
          if (font_faces[i]->GetWeight() == weight) {
            matching_face = font_faces[i];
            break;
          }
        }
        if (diff < max_difference) {
          matching_face = font_faces[i];
          max_difference = diff;
        } else if (diff == max_difference) {
          if (font_faces[i]->GetWeight() == weight) {
            matching_face = font_faces[i];
          }
        }
      } else {
        matching_face = font_faces[i];
        if (font_faces[i]->GetWeight() == weight && max_difference == 0) {
          break;
        }
      }
    }
  }

  if (matching_face == NULL)
    return NULL;

  return matching_face->GetHandle(charset, size);
}
}  // namespace Core
}  // namespace Rocket
