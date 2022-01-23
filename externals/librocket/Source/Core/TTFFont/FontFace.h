// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef ROCKETCORETTFFONTFACE_H
#define ROCKETCORETTFFONTFACE_H

#include "../../../Include/Rocket/Core/FontFace.h"
#include "TTFFontDefinitions.h"

namespace Rocket {
namespace Core {
class Context;
namespace TTFFont {
class TTFFontDefinitions;

class FontFaceHandle;

class FontFace : public Rocket::Core::FontFace {
public:
  FontFace(Context* _context, TTFFontDefinitions* _face);
  ~FontFace();

  Rocket::Core::FontFaceHandle* GetHandle(const String& charset, int size);
  void ReleaseFace();
  int GetFaceSize() {
    return -1;
  }

private:
  TTFFontDefinitions* m_face;
};
}  // namespace TTFFont
}  // namespace Core
}  // namespace Rocket

#endif
