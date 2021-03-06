/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef ROCKETDEBUGGERELEMENTLOG_H
#define ROCKETDEBUGGERELEMENTLOG_H

#include <deque>
#include "../../Include/Rocket/Core/ElementDocument.h"
#include "../../Include/Rocket/Core/EventListener.h"

namespace Rocket {
namespace Core {
class Context;
}
namespace Debugger {

class SystemInterface;

/**
        @author Robert Curry
 */

class ElementLog : public Core::ElementDocument, public Core::EventListener {
public:
  ROCKET_RTTI_DefineWithParent(Core::ElementDocument);
  ElementLog(Core::Context* _context, const Core::String& tag);
  virtual ~ElementLog();

  /// Initialises the log element.
  /// @return True if the element initialised successfully, false otherwise.
  bool Initialise();

  /// Adds a log message to the debug log.
  void AddLogMessage(Core::Log::Type type, const Core::String& message);

protected:
  virtual void OnRender() override;
  virtual void ProcessEvent(Core::Event& event) override;

private:
  struct LogMessage {
    unsigned int index;
    Core::String message;
  };
  typedef std::deque<LogMessage> LogMessageList;

  struct LogType {
    bool visible;
    Core::String class_name;
    Core::String alert_contents;
    Core::String button_name;
    LogMessageList log_messages;
  };
  LogType log_types[Core::Log::LT_MAX];

  int FindNextEarliestLogType(unsigned int log_pointers[Core::Log::LT_MAX]);

  unsigned int current_index;
  bool dirty_logs;
  bool auto_scroll;
  Core::Element* message_content;
  Core::ElementDocument* beacon;
  int current_beacon_level;
};
}  // namespace Debugger
}  // namespace Rocket

#endif
