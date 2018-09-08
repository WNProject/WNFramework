// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplicationData/inc/WNApplicationData.h"

using namespace wn;

int32_t wn_application_main(
    const runtime::application::application_data* _application_data) {
  _application_data->default_log->set_log_level(logging::log_level::info);
  _application_data->default_log->log_info("Engine bootstrap startup.");

  // TODO: Add stuff here

  _application_data->default_log->log_info("Engine bootstrap shutdown.");

  return 0;
}
