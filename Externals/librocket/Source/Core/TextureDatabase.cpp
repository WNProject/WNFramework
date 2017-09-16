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
#include "TextureDatabase.h"
#include "../../Include/Rocket/Core.h"
#include "../../Include/Rocket/Core/Context.h"
#include "TextureResource.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

static TextureDatabase** kTextureDatabaseInstance;

void TextureDatabase::RegisterContextValues(Context* _context) {
  _context->RegisterCachedType(kTextureDatabaseInstance);
}

TextureDatabase::TextureDatabase(Context* _context) : m_context(_context) {
  auto& instance = (*_context)(kTextureDatabaseInstance);
  ROCKET_ASSERT(instance == NULL);
  instance = this;
}

TextureDatabase::~TextureDatabase() {
  auto& instance = (*m_context)(kTextureDatabaseInstance);
  ROCKET_ASSERT(instance == this);
  instance = NULL;
}

void TextureDatabase::Initialise(Context* _context) {
  new TextureDatabase(_context);
}

void TextureDatabase::Shutdown(Context* _context) {
  auto& instance = (*_context)(kTextureDatabaseInstance);
  delete instance;
}

// If the requested texture is already in the database, it will be returned with
// an extra reference count. If not, it
// will be loaded through the application's render interface.
TextureResource* TextureDatabase::Fetch(
    Context* _context, const String& source, const String& source_directory) {
  auto& instance = (*_context)(kTextureDatabaseInstance);
  String path;
  if (source.Substring(0, 1) == "?")
    path = source;
  else
    GetSystemInterface(_context)->JoinPath(
        path, source_directory.Replace("|", ":"), source);

  TextureMap::iterator iterator = instance->textures.find(path);
  if (iterator != instance->textures.end()) {
    (*iterator).second->AddReference();
    return (*iterator).second;
  }

  TextureResource* resource = new TextureResource(_context);
  if (!resource->Load(path)) {
    resource->RemoveReference();
    return NULL;
  }

  instance->textures[resource->GetSource()] = resource;
  return resource;
}

// Releases all textures in the database.
void TextureDatabase::ReleaseTextures(Context* _context) {
  auto& instance = (*_context)(kTextureDatabaseInstance);
  for (TextureMap::iterator i = instance->textures.begin();
       i != instance->textures.end(); ++i)
    i->second->Release();
}

// Removes a texture from the database.
void TextureDatabase::RemoveTexture(
    Context* _context, TextureResource* texture) {
  auto& instance = (*_context)(kTextureDatabaseInstance);
  if (instance != NULL) {
    TextureMap::iterator iterator =
        instance->textures.find(texture->GetSource());
    if (iterator != instance->textures.end())
      instance->textures.erase(iterator);
  }
}

// Release all textures bound through a render interface.
void TextureDatabase::ReleaseTextures(
    Context* _context, RenderInterface* render_interface) {
  auto& instance = (*_context)(kTextureDatabaseInstance);
  if (instance != NULL) {
    for (TextureMap::iterator i = instance->textures.begin();
         i != instance->textures.end(); ++i)
      i->second->Release(render_interface);
  }
}
}  // namespace Core
}  // namespace Rocket
