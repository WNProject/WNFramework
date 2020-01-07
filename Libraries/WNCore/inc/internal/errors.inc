// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// clang-format off
    ok,                       // Everything is fine
    error,                // Unspecified error
    invalid_parameters,   // Some parameter was invalid
    aborted,              // We aborted running (Maybe we initiated a close)
    eWNOutOfRange,        // A value was out of range
    eWNBadType,           // The type passed in was not acceptable
    does_not_exist,       // The requested resource does not exist
    cannot_open_file,     // We cannot open the given file
    eWNPlatformError,     // There was a non-specific platform error
    already_initialized,  // Something is trying to be initialized twice
    uninitialized,        // An object is being used while uninitialized
    eWNInvalidFile,       // The file is not valid
    eUnsupported,         // The operation is not supported