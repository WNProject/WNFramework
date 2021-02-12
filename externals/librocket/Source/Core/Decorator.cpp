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
#include "../../Include/Rocket/Core/Decorator.h"
#include "../../Include/Rocket/Core/DecoratorInstancer.h"
#include "../../Include/Rocket/Core/PropertyDefinition.h"
#include "TextureDatabase.h"
#include "TextureResource.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

Decorator::Decorator(Context* _context) : m_context(_context) {
  instancer = NULL;
  z_index = 0;
  specificity = -1;
}

Decorator::~Decorator() {}

// Sets the z-index of the decorator.
void Decorator::SetZIndex(float _z_index) {
  z_index = _z_index;
}

// Returns the decorator's z-index.
float Decorator::GetZIndex() const {
  return z_index;
}

// Sets the specificity of the decorator.
void Decorator::SetSpecificity(int _specificity) {
  specificity = _specificity;
}

// Returns the specificity of the decorator.
int Decorator::GetSpecificity() const {
  return specificity;
}

// Releases the decorator through its instancer.
void Decorator::OnReferenceDeactivate() {
  if (instancer != NULL)
    instancer->ReleaseDecorator(this);
}

// Attempts to load a texture into the list of textures in use by the decorator.
int Decorator::LoadTexture(
    const String& texture_name, const String& rcss_path) {
  for (size_t i = 0; i < textures.size(); i++) {
    if (texture_name == textures[i].GetSource())
      return (int)i;
  }

  Texture texture(m_context);
  if (!texture.Load(texture_name, rcss_path))
    return -1;

  textures.push_back(texture);
  return (int)textures.size() - 1;
}

// Returns one of the decorator's previously loaded textures.
const Texture* Decorator::GetTexture(int index) const {
  if (index < 0 || index >= (int)textures.size())
    return NULL;

  return &(textures[index]);
}

// Returns the floating-point value of a numerical property from a dictionary of
// properties.
float Decorator::ResolveProperty(const PropertyDictionary& properties,
    const String& name, float base_value) const {
  const Property* property = properties.GetProperty(name);
  if (property == NULL) {
    ROCKET_ERROR;
    return 0;
  }

  // Need to include em!
  if (property->unit & Property::RELATIVE_UNIT)
    return base_value * property->value.Get<float>() * 0.01f;

  if (property->unit & Property::NUMBER || property->unit & Property::PX)
    return property->value.Get<float>();

  // Values based on pixels-per-inch.
  if (property->unit & Property::PPI_UNIT) {
    float inch = property->value.Get<float>() *
                 GetRenderInterface(m_context)->GetPixelsPerInch();

    if (property->unit & Property::INCH)  // inch
      return inch;
    if (property->unit & Property::CM)  // centimeter
      return inch / 2.54f;
    if (property->unit & Property::MM)  // millimeter
      return inch / 25.4f;
    if (property->unit & Property::PT)  // point
      return inch / 72.0f;
    if (property->unit & Property::PC)  // pica
      return inch / 6.0f;
  }

  ROCKET_ERROR;
  return 0;
}
}  // namespace Core
}  // namespace Rocket
