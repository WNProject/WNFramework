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

// clang-format off

#ifndef ROCKETCORESTYLESHEETFACTORY_H
#define ROCKETCORESTYLESHEETFACTORY_H

#include "../../Include/Rocket/Core/Types.h"

namespace Rocket {
namespace Core {

class Context;
class StyleSheet;
class StyleSheetNodeSelector;

/**
        Creates stylesheets on the fly as needed. The factory keeps a cache of
   built sheets for optimisation.

        @author Lloyd Weehuizen
 */

class StyleSheetFactory {
public:
  /// Initialise the style factory
  static bool Initialise(Context* _context);
  /// Shutdown style manager
  static void Shutdown(Context* _context);

  /// Gets the named sheet, retrieving it from the cache if its already been
  /// loaded
  /// @param sheet name of sheet to load
  static StyleSheet* GetStyleSheet(Context* _context, const String& sheet);

  /// Builds and returns a stylesheet based on the list of input sheets
  /// Generated sheets will be cached for later use
  /// @param sheets List of sheets to combine into one
  static StyleSheet* GetStyleSheet(Context* _context, const StringList& sheets);

  /// Clear the style sheet cache.
  static void ClearStyleSheetCache(Context* _context);

  /// Returns one of the available node selectors.
  /// @param name[in] The name of the desired selector.
  /// @return The selector registered with the given name, or NULL if none
  /// exists.
  static StyleSheetNodeSelector* GetSelector(
      Context* _context, const String& name);

  static void RegisterContextValues(Context* _context);

private:
  StyleSheetFactory(Context* _context);
  ~StyleSheetFactory();

  // Loads an individual style sheet
  StyleSheet* LoadStyleSheet(const String& sheet);

  // Individual loaded stylesheets
  typedef std::map<String, StyleSheet*> StyleSheets;
  StyleSheets stylesheets;

  // Cache of combined style sheets
  StyleSheets stylesheet_cache;

  // Custom complex selectors available for style sheets.
  typedef std::map<String, StyleSheetNodeSelector*> SelectorMap;
  SelectorMap selectors;

  Context* m_context;
};
}  // namespace Core
}  // namespace Rocket

#endif
