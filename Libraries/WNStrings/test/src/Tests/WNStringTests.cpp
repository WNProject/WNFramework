// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNStrings/test/inc/Common.h"
#include "WNStrings/inc/WNStrings.h"

TEST(Strings, basic_strings) {
	EXPECT_EQ(2, WNStrings::WNStrLen("ab"));
}
