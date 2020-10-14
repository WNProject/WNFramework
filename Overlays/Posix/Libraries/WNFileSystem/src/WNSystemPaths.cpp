#include "WNFileSystem/src/WNSystemPaths.h"

#include "WNContainers/inc/WNString.h"
#include "WNCore/inc/types.h"
#include "WNFileSystem/src/WNSystemTempPath.h"

#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace wn {
namespace executable {

struct executable_data;

}  // namespace executable

namespace file_system {
namespace internal {

containers::string get_scratch_path(memory::allocator* _allocator,
    const executable::executable_data* _executable_data) {
  const containers::string temp_path(
      get_temp_path(_allocator, _executable_data));

  if (!temp_path.empty()) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static const size_t length = 16;
    char name_template[length + 1];

    name_template[length] = 0;

    for (;;) {
      for (size_t i = 0; i < length; ++i) {
        name_template[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
      }

      containers::string path(temp_path + "scratch." + name_template + "/");

      if (::mkdir(path.data(), S_IRWXU | S_IRGRP | S_IXGRP) == 0) {
        return path;
      } else if (errno == EEXIST) {
        continue;
      }

      break;
    }
  }

  return nullptr;
}

containers::string get_current_working_path(memory::allocator* _allocator,
    const executable::executable_data* _executable_data) {
  containers::dynamic_array<char> buffer(_allocator);
  size_t size = static_cast<size_t>(PATH_MAX);

  for (;;) {
    buffer.resize(size);

    const char* result = ::getcwd(buffer.data(), buffer.size());

    if (result == NULL) {
      if (errno == ERANGE) {
        size *= 2;
      } else {
        return nullptr;
      }
    } else if (*result == '(' || *result != '/') {
      return nullptr;
    } else {
      return containers::string(_allocator, buffer.data());
    }
  }
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
