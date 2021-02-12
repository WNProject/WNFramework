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
#ifndef ROCKETCOREPLUGINREGISTRY_H
#define ROCKETCOREPLUGINREGISTRY_H

#include "../../Include/Rocket/Core/String.h"

namespace Rocket {
namespace Core {

class Context;
class DocumentContext;
class Element;
class ElementDocument;
class Plugin;

/**
        @author Peter Curry
 */

class PluginRegistry {
public:
  static void RegisterPlugin(Core::Context* _context, Plugin* plugin);

  /// Calls OnInitialise() on all plugins.
  static void NotifyInitialise(Core::Context* _context);
  /// Calls OnShutdown() on all plugins.
  static void NotifyShutdown(Core::Context* _context);

  /// Calls OnContextCreate() on all plugins.
  static void NotifyContextCreate(
      Core::Context* _context, DocumentContext* context);
  /// Calls OnContextDestroy() on all plugins.
  static void NotifyContextDestroy(
      Core::Context* _context, DocumentContext* context);

  /// Calls OnDocumentOpen() on all plugins.
  static void NotifyDocumentOpen(Core::Context* _context,
      DocumentContext* context, const String& document_path);
  /// Calls OnDocumentLoad() on all plugins.
  static void NotifyDocumentLoad(
      Core::Context* _context, ElementDocument* document);
  /// Calls OnDocumentUnload() on all plugins.
  static void NotifyDocumentUnload(
      Core::Context* _context, ElementDocument* document);

  /// Calls OnElementCreate() on all plugins.
  static void NotifyElementCreate(Core::Context* _context, Element* element);
  /// Calls OnElementDestroy() on all plugins.
  static void NotifyElementDestroy(Core::Context* _context, Element* element);

  static void RegisterContextValues(Context* _context);

private:
  PluginRegistry();
};
}  // namespace Core
}  // namespace Rocket

#endif
