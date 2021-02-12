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
#ifndef ROCKETCOREDECORATORTILEDIMAGE_H
#define ROCKETCOREDECORATORTILEDIMAGE_H

#include "DecoratorTiled.h"

namespace Rocket {
namespace Core {

/**
        @author Peter Curry
 */

class DecoratorTiledImage : public DecoratorTiled {
public:
  DecoratorTiledImage(Context* _context);
  virtual ~DecoratorTiledImage();

  /// Initialises the tile for the decorator.
  /// @param tile[in] The declaration for the tile.
  /// @param texture_name[in] The application-specific path to the texture for
  /// the tile.
  /// @param rcss_path[in] The path to the RCSS file that defined the texture
  /// source.
  /// @return True if the image loaded (or are pending loading) and are of
  /// compatible sizes, false otherwise.
  bool Initialise(
      const Tile& tiles, const String& texture_names, const String& rcss_path);

  /// Called on a decorator to generate any required per-element data for a
  /// newly decorated element.
  virtual DecoratorDataHandle GenerateElementData(Element* element) override;
  /// Called to release element data generated by this decorator.
  virtual void ReleaseElementData(DecoratorDataHandle element_data) override;

  /// Called to render the decorator on an element.
  virtual void RenderElement(
      Element* element, DecoratorDataHandle element_data) override;

private:
  Tile tile;
};
}  // namespace Core
}  // namespace Rocket

#endif
