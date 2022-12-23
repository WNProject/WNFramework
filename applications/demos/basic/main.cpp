#include "WNApplicationData/inc/WNApplicationData.h"

using namespace wn;

int32_t wn_application_main(
    const runtime::application::application_data* _data) {
  _data->default_log->log_error("ERROR!!!!!");
  return -1;
}