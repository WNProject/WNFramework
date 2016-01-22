// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "WNContainers/inc/WNStringView.h"

namespace wn {
namespace file_system {
namespace internal {

#ifdef _WN_WINDOWS
static const char* tokens = "..\\/";
#elif defined _WN_POSIX
static const char* tokens = "../\\";
#endif

static const containers::string_view good_slash(tokens, 2, 1);
static const containers::string_view dot_dot(tokens, 2);
static const containers::string_view dot_dot_slash(tokens, 3);

WN_INLINE void sanitize_path(containers::string& _path) {
  if (!_path.empty()) {
    if (_path == ".") {
      _path.clear();
    } else if (_path == "..") {
      _path.assign(dot_dot_slash.data(), dot_dot_slash.size());
    } else {
      static const containers::string_view bad_slash(tokens, 3, 1);

      if (_path.size() >= 2) {
        const char last_char = _path.back();
        const char second_last_char = _path[_path.size() - 2];

        if (last_char == '.' && (second_last_char == good_slash.front() ||
                                    second_last_char == bad_slash.front())) {
          _path += good_slash.front();
        } else if (_path.size() >= 3) {
          const char third_last_char = _path[_path.size() - 3];

          if (last_char == '.' && second_last_char == '.' &&
              (third_last_char == good_slash.front() ||
                  third_last_char == bad_slash.front())) {
            _path += good_slash.front();
          }
        }
      }

      static const containers::string_view dot_slash(tokens, 1, 2);
      static const containers::string_view find_slash_filter(tokens, 2, 2);
      char& front = _path.front();

      if (front == bad_slash.front()) {
        front = good_slash.front();
      }

      const size_t initial_pos = (front == good_slash.front()) ? 1 : 0;
      size_t pos = initial_pos;
      size_t last_pos = initial_pos;
      containers::string_view last_view;

      while ((pos = _path.find_first_of(find_slash_filter.to_string(), pos)) !=
             containers::string::npos) {
        char& current = _path[pos];

        if (current == bad_slash.front()) {
          current = good_slash.front();
        }

        pos++;

        const char* current_token_start = _path.data() + last_pos;
        const char* current_token_end = _path.data() + pos;
        containers::string_view view(current_token_start, current_token_end);

        if (view == good_slash || view == dot_slash) {
          const size_t token_pos = view.data() - _path.data();
          const size_t token_size = view.size();
          const size_t last_token_pos = last_view.data() - _path.data();
          const size_t last_token_size = last_view.size();

          _path.erase(token_pos, token_size);

          pos -= token_size;
          last_view = containers::string_view(
              _path.data() + last_token_pos, last_token_size);
        } else if (view == dot_dot_slash && !last_view.empty() &&
                   last_view != dot_dot_slash) {
          const size_t token_pos = view.data() - _path.data();
          const size_t token_size = view.size();
          const size_t last_token_pos = last_view.data() - _path.data();
          const size_t last_token_size = last_view.size();

          _path.erase(token_pos, token_size);
          _path.erase(last_token_pos, last_token_size);

          if (last_token_pos != 0) {
            if ((last_pos = _path.rfind(good_slash.front(),
                     last_token_pos - 2)) == containers::string::npos) {
              last_pos = initial_pos;
            } else {
              last_pos++;
            }

            pos = last_token_pos;
            last_view = containers::string_view(
                _path.data() + last_pos, _path.data() + last_token_pos);
            last_pos += last_view.size();
          } else {
            last_pos = initial_pos;
            pos = initial_pos;

            last_view.clear();
          }
        } else {
          last_view = std::move(view);
          last_pos = pos;
        }
      }
    }
  }
}

WN_INLINE bool validate_relative_path(containers::string_view _view) {
  if (_view.empty() || _view == good_slash) {
    return false;
  } else if (_view.size() >= 2) {
    _view.remove_suffix(_view.size() - 2);

    if (_view == dot_dot) {
      return false;
    }

    _view.remove_suffix(1);

    if (_view == good_slash) {
      return false;
    }
  }

  return true;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
