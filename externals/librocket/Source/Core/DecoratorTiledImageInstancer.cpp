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
// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#include "DecoratorTiledImageInstancer.h"
#include "DecoratorTiledImage.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

DecoratorTiledImageInstancer::DecoratorTiledImageInstancer(Context* _context)
  : DecoratorTiledInstancer(_context) {
  RegisterTileProperty("image", false);
  RegisterProperty("color-multiplier", "white").AddParser(COLOR);
}

DecoratorTiledImageInstancer::~DecoratorTiledImageInstancer() {}

// Instances a box decorator.
Decorator* DecoratorTiledImageInstancer::InstanceDecorator(
    const String& ROCKET_UNUSED_PARAMETER(name),
    const PropertyDictionary& _properties) {
  DecoratorTiled::Tile tile;
  String texture_name;
  String rcss_path;

  GetTileProperties(tile, texture_name, rcss_path, _properties, "image");

  DecoratorTiledImage* decorator = new DecoratorTiledImage(m_context);

  decorator->SetColorMultiplier(
      _properties.GetProperty("color-multiplier")->value.Get<Colourb>());

  if (decorator->Initialise(tile, texture_name, rcss_path))
    return decorator;

  decorator->RemoveReference();
  ReleaseDecorator(decorator);
  return NULL;
}

// Releases the given decorator.
void DecoratorTiledImageInstancer::ReleaseDecorator(Decorator* decorator) {
  delete decorator;
}

// Releases the instancer.
void DecoratorTiledImageInstancer::Release() {
  delete this;
}
}  // namespace Core
}  // namespace Rocket
