// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_ERRORS_H__
#define __WN_SCRIPTING_ERRORS_H__

enum eWNTypeError {
#include "WNCore/inc/Internal/WNErrors.inc"
  eWNAlreadyExists,
  eWNInvalidName,
  eWNInvalidConstant,
  eWNInvalidCast,
  eWNCannotCreateType,
  eWNInvalidOperation,
  eWNAmbiguous,
  eWNPartiallyAmbiguous,
  eWNUnknownResolution,
  eWNNoVirtual
};

#endif  //__WN_SCRIPTING_ERRORS_H__
