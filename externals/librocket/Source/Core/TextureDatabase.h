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
#ifndef ROCKETCORETEXTUREDATABASE_H
#define ROCKETCORETEXTUREDATABASE_H

#include <map>
#include "../../Include/Rocket/Core/String.h"

namespace Rocket {
namespace Core {

class Context;
class RenderInterface;
class TextureResource;

/**
        @author Peter Curry
 */

class TextureDatabase {
public:
  static void Initialise(Context* _context);
  static void Shutdown(Context* _context);

  /// If the requested texture is already in the database, it will be returned
  /// with an extra
  /// reference count. If not, it will be loaded through the application's
  /// render interface.
  static TextureResource* Fetch(
      Context* _context, const String& source, const String& source_directory);

  /// Releases all textures in the database.
  static void ReleaseTextures(Context* _context);

  /// Removes a texture from the database.
  static void RemoveTexture(Context* _context, TextureResource* texture);

  /// Release all textures bound through a render interface.
  static void ReleaseTextures(
      Context* _context, RenderInterface* render_interface);

  static void RegisterContextValues(Context* _context);

private:
  TextureDatabase(Context* _context);
  ~TextureDatabase();

  typedef std::map<String, TextureResource*> TextureMap;
  TextureMap textures;

  Context* m_context;
};
}  // namespace Core
}  // namespace Rocket

#endif
