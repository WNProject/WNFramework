// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "platform_utils/inc/subprocess.h"

#include <fcntl.h>
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>

#include "WNContainers/inc/WNStringView.h"

namespace wn {
namespace runtime {
namespace platform_utils {

subprocess_return call_subprocess(memory::allocator* _allocator,
    const containers::string_view _application,
    const containers::string_view _input,
    const containers::contiguous_range<const containers::string_view> _args) {
  int stdout_pipe[2];
  int stderr_pipe[2];
  int stdin_pipe[2];

  if (pipe(stdout_pipe) < 0) {
    return subprocess_return(containers::string(), containers::string(),
        subprocess_error::cannot_create_pipe, 0);
  }

  if (pipe(stderr_pipe) < 0) {
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
    return subprocess_return(containers::string(), containers::string(),
        subprocess_error::cannot_create_pipe, 0);
  }

  if (!_input.empty()) {
    if (pipe(stdin_pipe) < 0) {
      close(stdout_pipe[0]);
      close(stdout_pipe[1]);
      close(stderr_pipe[0]);
      close(stderr_pipe[1]);
      return subprocess_return(containers::string(), containers::string(),
          subprocess_error::cannot_create_pipe, 0);
    }
  }

  containers::dynamic_array<char*> args(_allocator);
  args.reserve(1 + _args.size());

  containers::dynamic_array<containers::string> args_strings(_allocator);
  args_strings.push_back(_application.to_string(_allocator));

  for (auto& a : _args) {
    args_strings.push_back(a.to_string(_allocator));
  }

  for (auto& a : args_strings) {
    args.push_back(a.data());
  }

  args.push_back(nullptr);
  int child = fork();
  if (!child) {
    if (dup2(stdout_pipe[1], STDOUT_FILENO) == -1) {
      exit(errno);
    }
    if (dup2(stderr_pipe[1], STDERR_FILENO) == -1) {
      exit(errno);
    }
    if (!_input.empty()) {
      if (dup2(stdin_pipe[0], STDIN_FILENO) == -1) {
        exit(errno);
      }
      close(stdin_pipe[0]);
      close(stdin_pipe[1]);
    }

    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
    close(stderr_pipe[0]);
    close(stderr_pipe[1]);
    int ret = execv(args[0], args.data());
    exit(ret);
  }

  close(stdout_pipe[1]);
  close(stderr_pipe[1]);

  if (!_input.empty()) {
    close(stdin_pipe[0]);
    size_t offset = 0;
    bool success = true;
    while (success && (offset != _input.size())) {
      size_t to_write = _input.size() - offset;
      if (to_write > 4096) {
        to_write = 4096;
      }
      size_t written = write(stdin_pipe[1], _input.data() + offset, to_write);
      if (written > 0) {
        offset += written;
      }
      success = (written > 0) || (errno == EAGAIN);
    }
    close(stdin_pipe[1]);
  }

  fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK);
  fcntl(stderr_pipe[0], F_SETFL, O_NONBLOCK);
  pollfd fds[2];
  fds[0].fd = stdout_pipe[0];
  fds[1].fd = stderr_pipe[0];
  fds[0].events = POLLHUP | POLLIN;
  fds[1].events = POLLHUP | POLLIN;

  containers::string outs[2] = {
      containers::string(_allocator), containers::string(_allocator)};
  char read_buff[4096];
  while (true) {
    int ret = poll(fds, 2, 50);
    if (ret <= 0) {
      switch (errno) {
        case EAGAIN:
        case EINTR:
          continue;
      }
      break;
    }
    bool done = false;
    for (size_t i = 0; !done && (i < 2); ++i) {
      if (fds[i].revents & POLLIN) {
        ssize_t rd = read(fds[i].fd, read_buff, 4096);
        if (rd < 0) {
          if (errno != EAGAIN) {
            done = true;
            break;
          }
        } else {
          outs[i].append(read_buff, rd);
        }
      }
    }
    for (size_t i = 0; !done && (i < 2); ++i) {
      if (fds[i].revents & POLLHUP) {
        done = true;
        break;
      }
    }
    if (done) {
      break;
    }
  }

  close(stdout_pipe[0]);
  close(stderr_pipe[0]);

  int status;
  waitpid(child, &status, 0);

  if (WIFEXITED(status)) {
    status = WEXITSTATUS(status);
  }

  return subprocess_return(core::move(outs[0]), core::move(outs[1]),
      status == 0 ? subprocess_error::ok
                  : subprocess_error::process_exited_uncleanly,
      static_cast<uint32_t>(status));
}

}  // namespace platform_utils
}  // namespace runtime
}  // namespace wn
