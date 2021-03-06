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
#ifndef ROCKETCOREDECORATORNONEINSTANCER_H
#define ROCKETCOREDECORATORNONEINSTANCER_H

#include "../../Include/Rocket/Core/DecoratorInstancer.h"

namespace Rocket {
namespace Core {
class Context;
/**
        The instancer for the none decorator.

        @author Peter Curry
 */

class DecoratorNoneInstancer : public DecoratorInstancer {
public:
  DecoratorNoneInstancer(Context* _context);
  virtual ~DecoratorNoneInstancer();

  /// Instances a decorator given the property tag and attributes from the RCSS
  /// file.
  /// @param name The type of decorator desired. For example,
  /// "background-decorator: simple;" is declared as type "simple".
  /// @param properties All RCSS properties associated with the decorator.
  /// @return The decorator if it was instanced successful, NULL if an error
  /// occured.
  virtual Decorator* InstanceDecorator(
      const String& name, const PropertyDictionary& properties);
  /// Releases the given decorator.
  /// @param decorator Decorator to release. This is guaranteed to have been
  /// constructed by this instancer.
  virtual void ReleaseDecorator(Decorator* decorator);

  /// Releases the instancer.
  virtual void Release();
};
}  // namespace Core
}  // namespace Rocket

#endif
