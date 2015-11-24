// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/WNTypes.h"
#include "WNUtils/inc/WNCrashHandler.h"

extern wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]);

void wn_dummy() {}

int main(int _argc, char** _argv) {
    WNUtils::InitializeCrashHandler();

    return(wn_main(_argc, _argv));
}