// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "WNContainers/inc/WNDynamicArray.h"
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

WN_INLINE void append_path(
    containers::string& _path, containers::string_view _appendee) {
  if (!_path.empty() && _path.back() != good_slash.data()[0]) {
    _path.append(good_slash.data(), good_slash.size());
  }

  _path.append(_appendee.data(), _appendee.size());
}

WN_INLINE void sanitize_path(containers::string& _path) {
  if (_path.empty()) {
    return;
  }

  if (_path == ".") {
    _path.clear();
  } else if (_path == "..") {
    _path.assign(dot_dot_slash.data(), dot_dot_slash.size());
  } else {
    static const containers::string_view bad_slash(tokens, 3, 1);

    if (_path.size() >= 2) {
      const char last_char = _path.back();
      const char second_last_char = _path[_path.size() - 2];

      if (last_char == '.' && (second_last_char == good_slash[0] ||
                                  second_last_char == bad_slash[0])) {
        _path += good_slash[0];
      } else if (_path.size() >= 3) {
        const char third_last_char = _path[_path.size() - 3];

        if (last_char == '.' && second_last_char == '.' &&
            (third_last_char == good_slash[0] ||
                third_last_char == bad_slash[0])) {
          _path += good_slash[0];
        }
      }
    }

    static const containers::string_view dot_slash(tokens, 1, 2);
    static const containers::string_view find_slash_filter(tokens, 2, 2);
    char& front = _path[0];

    if (front == bad_slash[0]) {
      front = good_slash[0];
    }

    const size_t initial_pos = (front == good_slash[0]) ? 1 : 0;
    size_t pos = initial_pos;
    size_t last_pos = initial_pos;
    containers::string_view last_view;

    while ((pos = _path.find_first_of(find_slash_filter.data(), pos,
                find_slash_filter.size())) != containers::string::npos) {
      char& current = _path[pos];

      if (current == bad_slash[0]) {
        current = good_slash[0];
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
          if ((last_pos = _path.rfind(good_slash[0], last_token_pos - 2)) ==
              containers::string::npos) {
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

// Gets the directory portion of a path. If there are no
// path separators in the path. The portion of the _path
// that comes after the last path separator is considered
// to be the file name, and is ignored. Therefore if there
// is no path separator, then an empty string_view is returend.
WN_INLINE bool get_directory_from_sanitized_path(
    const containers::string_view _path, containers::string_view& _directory) {
  if (_path.empty()) {
    _directory = containers::string_view();

    return true;
  }

  for (size_t i = _path.size(); i > 0; --i) {
    if (_path[i - 1] == good_slash[0]) {
      _directory = containers::string_view(&_path[0], &_path[i - 1]);

      return true;
    }
  }

  _directory = containers::string_view();

  return true;
}

// Gets the filename portion of a path. This is the
// last piece after the trailing path separator.
// If there is no separator, then it is the entire path.
WN_INLINE bool get_filename_from_sanitized_path(
    const containers::string_view _path, containers::string_view& _file) {
  if (_path.empty()) {
    _file = containers::string_view();

    return true;
  }

  for (size_t i = _path.size(); i > 0; --i) {
    if (_path[i - 1] == good_slash[0]) {
      _file = containers::string_view(
          &_path[i - 1] + 1, &_path[_path.size() - 1] + 1);

      return true;
    }
  }

  _file = _path;

  return true;
}

// Spits a sanitized path into its components. Each path component
// is appended to the _path_pieces dynamic_array. Empty components
// are removed.
WN_INLINE bool split_sanitized_path(const containers::string_view _path,
    containers::dynamic_array<containers::string_view>& _path_pieces) {
  if (_path.empty()) {
    return true;
  }

  size_t start_of_path = 0;
  size_t i = 0;

  for (; i < _path.size(); ++i) {
    if (_path[i] == good_slash[0]) {
      if (start_of_path != i) {
        _path_pieces.emplace_back(&_path[start_of_path], &_path[i]);
      }

      start_of_path = i + 1;
    }
  }

  if (i != start_of_path) {
    _path_pieces.emplace_back(&_path[start_of_path], (&_path[i - 1]) + 1);
  }

  return true;
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
