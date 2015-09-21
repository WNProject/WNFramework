// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMemory/test/inc/Common.h"

wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]) {
    testing::InitGoogleTest(&_argc, _argv);

    const wn_uint32 result = RUN_ALL_TESTS();

    return(result);
}