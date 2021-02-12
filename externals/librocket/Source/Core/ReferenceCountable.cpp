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
#include "../../Include/Rocket/Core/ReferenceCountable.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

// Constructor.
ReferenceCountable::ReferenceCountable(int initial_count) {
  reference_count = initial_count;
}

// Destructor. The reference count must be 0 when this is invoked.
ReferenceCountable::~ReferenceCountable() {
  ROCKET_ASSERT(reference_count == 0);
}

// Returns the number of references outstanding against this object.
int ReferenceCountable::GetReferenceCount() {
  return reference_count;
}

// Adds a reference to the object.
void ReferenceCountable::AddReference() {
  reference_count++;
  if (reference_count == 1) {
    OnReferenceActivate();
  }
}

// Removes a reference from the object.
void ReferenceCountable::RemoveReference() {
  ROCKET_ASSERT(reference_count > 0);
  reference_count--;
  if (reference_count == 0) {
    OnReferenceDeactivate();
  }
}

ReferenceCountable& ReferenceCountable::operator=(
    const ReferenceCountable& /*copy*/) {
  ROCKET_ERRORMSG(
      "Attempting to copy a reference counted object. This is not advisable.");
  return *this;
}

// A hook method called when the reference count climbs from 0.
void ReferenceCountable::OnReferenceActivate() {}

// A hook method called when the reference count drops to 0.
void ReferenceCountable::OnReferenceDeactivate() {}
}
}
