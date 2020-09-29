// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_UI_ELEMENT_DOCUMENT_H__
#define __WN_ENGINE_UI_ELEMENT_DOCUMENT_H__

#include "Rocket/Core/ElementDocument.h"
#include "Rocket/Core/ElementInstancer.h"

class element_document : public Rocket::Core::ElementDocument {
public:
  ROCKET_RTTI_DefineWithParent(Rocket::Core::ElementDocument);

  element_document(
      Rocket::Core::Context* _context, const Rocket::Core::String& tag);

  void LoadScript(
      Rocket::Core::Stream* stream, const Rocket::Core::String& source_name) {}

private:
};

class element_instancer : public Rocket::Core::ElementInstancer {}
#endif  // __WN_ENGINE_UI_ELEMENT_DOCUMENT_H__