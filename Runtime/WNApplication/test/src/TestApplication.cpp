// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplication/inc/WNApplicationData.h"

int32_t wn_application_main(
    const wn::runtime::application::application_data* _data) {
  _data->default_log->log_error("Hello World");
  return 0;
}
