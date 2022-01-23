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
#include "DecoratorTiledBoxInstancer.h"
#include "DecoratorTiledBox.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

DecoratorTiledBoxInstancer::DecoratorTiledBoxInstancer(Context* _context)
  : DecoratorTiledInstancer(_context) {
  RegisterTileProperty("top-left-image", false);
  RegisterTileProperty("top-right-image", false);
  RegisterTileProperty("bottom-left-image", false);
  RegisterTileProperty("bottom-right-image", false);

  RegisterTileProperty("left-image", true);
  RegisterTileProperty("right-image", true);
  RegisterTileProperty("top-image", true);
  RegisterTileProperty("bottom-image", true);

  RegisterTileProperty("center-image", true);

  RegisterProperty("color-multiplier", "white").AddParser(COLOR);
}

DecoratorTiledBoxInstancer::~DecoratorTiledBoxInstancer() {}

// Instances a box decorator.
Decorator* DecoratorTiledBoxInstancer::InstanceDecorator(
    const String& ROCKET_UNUSED_PARAMETER(name),
    const PropertyDictionary& _properties) {
  DecoratorTiled::Tile tiles[9];
  String texture_names[9];
  String rcss_paths[9];

  GetTileProperties(
      tiles[0], texture_names[0], rcss_paths[0], _properties, "top-left-image");
  GetTileProperties(tiles[1], texture_names[1], rcss_paths[1], _properties,
      "top-right-image");
  GetTileProperties(tiles[2], texture_names[2], rcss_paths[2], _properties,
      "bottom-left-image");
  GetTileProperties(tiles[3], texture_names[3], rcss_paths[3], _properties,
      "bottom-right-image");
  GetTileProperties(
      tiles[4], texture_names[4], rcss_paths[4], _properties, "left-image");
  GetTileProperties(
      tiles[5], texture_names[5], rcss_paths[5], _properties, "right-image");
  GetTileProperties(
      tiles[6], texture_names[6], rcss_paths[6], _properties, "top-image");
  GetTileProperties(
      tiles[7], texture_names[7], rcss_paths[7], _properties, "bottom-image");
  GetTileProperties(
      tiles[8], texture_names[8], rcss_paths[8], _properties, "center-image");

  DecoratorTiledBox* decorator = new DecoratorTiledBox(m_context);

  decorator->SetColorMultiplier(
      _properties.GetProperty("color-multiplier")->value.Get<Colourb>());

  if (decorator->Initialise(tiles, texture_names, rcss_paths))
    return decorator;

  decorator->RemoveReference();
  ReleaseDecorator(decorator);
  return NULL;
}

// Releases the given decorator.
void DecoratorTiledBoxInstancer::ReleaseDecorator(Decorator* decorator) {
  delete decorator;
}

// Releases the instancer.
void DecoratorTiledBoxInstancer::Release() {
  delete this;
}
}  // namespace Core
}  // namespace Rocket
