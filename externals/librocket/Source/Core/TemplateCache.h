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
#ifndef ROCKETCORETEMPLATECACHE_H
#define ROCKETCORETEMPLATECACHE_H

#include <map>
#include "../../Include/Rocket/Core/String.h"

namespace Rocket {
namespace Core {

class Context;
class Template;

/**
        Manages requests for loading templates, caching as it goes.

        @author Lloyd Weehuizen
 */

class TemplateCache {
public:
  /// Initialisation and Shutdown
  static bool Initialise(Context* _context);
  static void Shutdown(Context* _context);

  /// Load the named template from the given path, if its already loaded get the
  /// cached copy
  static Template* LoadTemplate(Context* _context, const String& path);
  /// Get the template by id
  static Template* GetTemplate(Context* _context, const String& id);

  /// Clear the template cache.
  static void Clear(Context* _context);

  static void RegisterContextValues(Context* _context);

private:
  TemplateCache(Context* _context);
  ~TemplateCache();

  typedef std::map<String, Template*> Templates;
  Templates templates;
  Templates template_ids;
  Context* m_context;
};
}  // namespace Core
}  // namespace Rocket

#endif
