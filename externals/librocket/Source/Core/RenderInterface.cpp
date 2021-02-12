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
#include "../../Include/Rocket/Core/RenderInterface.h"
#include "TextureDatabase.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

RenderInterface::RenderInterface(Context* _context)
  : ReferenceCountable(0), m_context(_context) {
  context = NULL;
}

RenderInterface::~RenderInterface() {}

// Called by Rocket when it wants to compile geometry it believes will be static
// for the forseeable future.
CompiledGeometryHandle RenderInterface::CompileGeometry(
    Vertex* ROCKET_UNUSED_PARAMETER(vertices),
    int ROCKET_UNUSED_PARAMETER(num_vertices),
    int* ROCKET_UNUSED_PARAMETER(indices),
    int ROCKET_UNUSED_PARAMETER(num_indices),
    TextureHandle ROCKET_UNUSED_PARAMETER(texture)) {
  return 0;
}

// Called by Rocket when it wants to render application-compiled geometry.
void RenderInterface::RenderCompiledGeometry(
    CompiledGeometryHandle ROCKET_UNUSED_PARAMETER(geometry),
    const Vector2f& ROCKET_UNUSED_PARAMETER(translation)) {}

// Called by Rocket when it wants to release application-compiled geometry.
void RenderInterface::ReleaseCompiledGeometry(
    CompiledGeometryHandle ROCKET_UNUSED_PARAMETER(geometry)) {}

// Called by Rocket when a texture is required by the library.
bool RenderInterface::LoadTexture(
    TextureHandle& ROCKET_UNUSED_PARAMETER(texture_handle),
    Vector2i& ROCKET_UNUSED_PARAMETER(texture_dimensions),
    const String& ROCKET_UNUSED_PARAMETER(source)) {
  return false;
}

// Called by Rocket when a texture is required to be built from an
// internally-generated sequence of pixels.
bool RenderInterface::GenerateTexture(
    TextureHandle& ROCKET_UNUSED_PARAMETER(texture_handle),
    const byte* ROCKET_UNUSED_PARAMETER(source),
    const Vector2i& ROCKET_UNUSED_PARAMETER(source_dimensions)) {
  return false;
}

// Called by Rocket when a loaded texture is no longer required.
void RenderInterface::ReleaseTexture(
    TextureHandle ROCKET_UNUSED_PARAMETER(texture)) {}

// Returns the native horizontal texel offset for the renderer.
float RenderInterface::GetHorizontalTexelOffset() {
  return 0;
}

// Returns the native vertical texel offset for the renderer.
float RenderInterface::GetVerticalTexelOffset() {
  return 0;
}

// Returns the number of pixels per inch.
float RenderInterface::GetPixelsPerInch() {
  return 100;
}

// Called when this render interface is released.
void RenderInterface::Release() {}

void RenderInterface::OnReferenceDeactivate() {
  TextureDatabase::ReleaseTextures(m_context, this);
  Release();
}

// Get the context currently being rendered.
DocumentContext* RenderInterface::GetContext() const {
  return context;
}
}  // namespace Core
}  // namespace Rocket
