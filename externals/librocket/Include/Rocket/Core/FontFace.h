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
#ifndef ROCKETCOREFONTFACE_H
#define ROCKETCOREFONTFACE_H

#include "Font.h"
#include "String.h"

#include <map>
#include <vector>

namespace Rocket {
namespace Core {
class Context;
class FontFaceHandle;

/**
        @author Peter Curry
 */

class FontFace {
public:
  FontFace(Context* _context, Font::Style style, Font::Weight weight,
      bool release_stream);
  virtual ~FontFace();

  /// Returns the style of the font face.
  /// @return The font face's style.
  Font::Style GetStyle() const;
  /// Returns the weight of the font face.
  /// @return The font face's weight.
  Font::Weight GetWeight() const;

  /// Returns a handle for positioning and rendering this face at the given
  /// size.
  /// @param[in] charset The set of characters in the handle, as a
  /// comma-separated list of unicode ranges.
  /// @param[in] size The size of the desired handle, in points.
  /// @return The shared font handle.
  virtual FontFaceHandle* GetHandle(const String& charset, int size) = 0;

  /// Releases the face's structure. This will mean handles for new sizes cannot
  /// be constructed,
  /// but existing ones can still be fetched.
  virtual void ReleaseFace() = 0;

  // Returns the font size if the font is only valid for a single size of text.
  // Returns -1 if this font can be used for any size of text.
  virtual int GetFaceSize() = 0;

protected:
  Font::Style style;
  Font::Weight weight;

  bool release_stream;

  typedef std::vector<FontFaceHandle*> HandleList;
  typedef std::map<int, HandleList> HandleMap;
  HandleMap handles;
  Context* m_context;
};
}  // namespace Core
}  // namespace Rocket

#endif
