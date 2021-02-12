// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#include "FontFace.h"
#include <Rocket/Core/Log.h>
#include "../precompiled.h"
#include "FontFaceHandle.h"
#include "TTFFontDefinitions.h"

namespace Rocket {
namespace Core {
namespace TTFFont {

FontFace::FontFace(Context* _context, TTFFontDefinitions* _face)
  : Rocket::Core::FontFace(
        _context, _face->Face.Style, _face->Face.Weight, true) {
  m_face = _face;
}

FontFace::~FontFace() {
  ReleaseFace();
}

// Returns a handle for positioning and rendering this face at the given size.
Rocket::Core::FontFaceHandle* FontFace::GetHandle(
    const String& _raw_charset, int size) {
  UnicodeRangeList charset;

  HandleMap::iterator iterator = handles.find(size);
  if (iterator != handles.end()) {
    const HandleList& hnds = (*iterator).second;

    // Check all the handles if their charsets match the requested one exactly
    // (ie, were specified by the same
    // string).
    String raw_charset(_raw_charset);
    for (size_t i = 0; i < handles.size(); ++i) {
      if (hnds[i]->GetRawCharset() == _raw_charset) {
        hnds[i]->AddReference();
        return (FontFaceHandle*)hnds[i];
      }
    }

    // Check all the handles if their charsets contain the requested charset.
    if (!UnicodeRange::BuildList(charset, raw_charset)) {
      Log::Message(m_context, Log::LT_ERROR, "Invalid font charset '%s'.",
          _raw_charset.CString());
      return NULL;
    }

    for (size_t i = 0; i < handles.size(); ++i) {
      bool range_contained = true;

      const UnicodeRangeList& handle_charset = hnds[i]->GetCharset();
      for (size_t j = 0; j < charset.size() && range_contained; ++j) {
        if (!charset[j].IsContained(handle_charset))
          range_contained = false;
      }

      if (range_contained) {
        hnds[i]->AddReference();
        return (FontFaceHandle*)hnds[i];
      }
    }
  }

  // See if this face has been released.
  if (m_face == NULL) {
    Log::Message(m_context, Log::LT_WARNING,
        "Font face has been released, unable to generate new handle.");
    return NULL;
  }

  // Construct and initialise the new handle.
  FontFaceHandle* handle = new FontFaceHandle(m_context);
  if (!handle->Initialise(m_face, _raw_charset, size)) {
    handle->RemoveReference();
    return NULL;
  }

  // Save the handle, and add a reference for the callee. The initial reference
  // will be removed when the font face
  // releases it.
  if (iterator != handles.end())
    (*iterator).second.push_back(handle);
  else
    handles[size] = HandleList(1, handle);

  handle->AddReference();

  return handle;
}

// Releases the face's structure.
void FontFace::ReleaseFace() {
  delete m_face;
}
}  // namespace TTFFont
}  // namespace Core
}  // namespace Rocket
