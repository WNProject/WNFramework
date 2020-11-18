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
#ifndef ROCKETCOREDECORATOR_H
#define ROCKETCOREDECORATOR_H

#include <vector>

#include "ReferenceCountable.h"
#include "Texture.h"
#include "Types.h"

namespace Rocket {
namespace Core {

class Context;
class DecoratorInstancer;
class Element;
class PropertyDictionary;
class Property;
struct Texture;
class TextureResource;

/**
        The abstract base class for any visual object that can be attached to
   any element.

        @author Peter Curry
 */

class Decorator : public ReferenceCountable {
public:
  Decorator(Context* _context);
  virtual ~Decorator();

  /// Called on a decorator to generate any required per-element data for a
  /// newly decorated element.
  /// @param[in] element The newly decorated element.
  /// @return A handle to a decorator-defined data handle, or NULL if none is
  /// needed for the element.
  virtual DecoratorDataHandle GenerateElementData(Element* element) = 0;
  /// Called to release element data generated by this decorator.
  /// @param[in] element_data The element data handle to release.
  virtual void ReleaseElementData(DecoratorDataHandle element_data) = 0;

  /// Sets the z-index of the decorator. A decorator with a higher z-index will
  /// be rendered after a decorator
  /// with a lower z-index. By default, all decorators have a z-index of 0.
  /// @param[in] z-index The new z-index of the decorator.
  void SetZIndex(float z_index);
  /// Returns the decorator's z-index.
  /// @return The z-index of the decorator.
  float GetZIndex() const;

  /// Sets the specificity of the decorator.
  /// @param[in] specificity The specificity of the decorator.
  void SetSpecificity(int specificity);
  /// Returns the specificity of the decorator. This is used when multiple
  /// pseudo-classes are active on an
  /// element, each with similarly-named decorators.
  /// @return The specificity of the decorator.
  int GetSpecificity() const;

  /// Called to render the decorator on an element.
  /// @param[in] element The element to render the decorator on.
  /// @param[in] element_data The handle to the data generated by the decorator
  /// for the element.
  virtual void RenderElement(
      Element* element, DecoratorDataHandle element_data) = 0;

  /// Value specifying an invalid or non-existent Decorator data handle.
  static const DecoratorDataHandle INVALID_DECORATORDATAHANDLE = 0;

protected:
  /// Releases the decorator through its instancer.
  virtual void OnReferenceDeactivate() override;

  /// Attempts to load a texture into the list of textures in use by the
  /// decorator.
  /// @param[in] texture_name The name of the texture to load.
  /// @param[in] rcss_path The RCSS file the decorator definition was loaded
  /// from; this is used to resolve relative paths.
  /// @return The index of the texture if the load was successful, or -1 if the
  /// load failed.
  int LoadTexture(const String& texture_name, const String& rcss_path);
  /// Returns one of the decorator's previously loaded textures.
  /// @param[in] index The index of the desired texture.
  /// @return The texture at the appropriate index, or NULL if the index was
  /// invalid.
  const Texture* GetTexture(int index = 0) const;

  /// Returns the floating-point value of a numerical property from a dictionary
  /// of properties, resolving it
  /// against a base value if it is a relative property.
  /// @param[in] properties The user-supplied dictionary of properties.
  /// @param[in] name The name of the desired property. This must be a numerical
  /// property.
  /// @return The fully-resolved value of the property, or 0 if an error
  /// occured.
  float ResolveProperty(const PropertyDictionary& properties,
      const String& name, float base_value) const;

private:
  DecoratorInstancer* instancer;

  // The z-index of this decorator, used to resolve render order when multiple
  // decorators are rendered
  // simultaneously on the same element.
  float z_index;
  // The maximum specificity of the properties used to define the decorator.
  int specificity;

  // Stores a list of textures in use by this decorator.
  std::vector<Texture> textures;

  Context* m_context;
  friend class Factory;
};
}  // namespace Core
}  // namespace Rocket

#endif