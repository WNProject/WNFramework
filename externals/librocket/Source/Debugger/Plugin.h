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
// found in the LICENSE.txt file.
#ifndef ROCKETDEBUGGERPLUGIN_H
#define ROCKETDEBUGGERPLUGIN_H

#include <set>
#include "../../Include/Rocket/Core/EventListener.h"
#include "../../Include/Rocket/Core/Plugin.h"

namespace Rocket {
namespace Core {

class Context;
class ElementDocument;
}  // namespace Core

namespace Debugger {

class ElementLog;
class ElementInfo;
class ElementContextHook;
class SystemInterface;

/**
        Rocket plugin interface for the debugger.

        @author Robert Curry
 */

class Plugin : public Core::Plugin, public Core::EventListener {
public:
  Plugin(Core::Context* _context);
  virtual ~Plugin();

  /// Initialises the debugging tools into the given context.
  /// @param[in] context The context to load the tools into.
  /// @return True on success, false if an error occured.
  bool Initialise(Core::DocumentContext* context);

  /// Sets the context to be debugged.
  /// @param[in] context The context to be debugged.
  /// @return True if the debugger is initialised and the context was switched,
  /// false otherwise..
  bool SetContext(Core::DocumentContext* context);

  /// Sets the visibility of the debugger.
  /// @param[in] visibility True to show the debugger, false to hide it.
  void SetVisible(bool visibility);
  /// Returns the visibility of the debugger.
  /// @return True if the debugger is visible, false if not.
  bool IsVisible();

  /// Renders any debug elements in the debug context.
  void Render();

  /// Called when Rocket shuts down.
  virtual void OnShutdown() override;

  /// Called whenever a Rocket context is destroyed.
  /// @param[in] context The destroyed context.
  virtual void OnContextDestroy(Core::DocumentContext* context) override;

  /// Called whenever an element is created.
  /// @param[in] element The created element.
  virtual void OnElementCreate(Core::Element* element) override;
  /// Called whenever an element is destroyed.
  /// @param[in] element The destroyed element.
  virtual void OnElementDestroy(Core::Element* element) override;

  /// Event handler for events from the debugger elements.
  /// @param[in] event The event to process.
  virtual void ProcessEvent(Core::Event& event) override;

  /// Access the singleton instance of the debugger
  /// @return NULL or an instance of the plugin
  static Plugin* GetInstance(Core::Context* _context);

  static void RegisterContextValues(Core::Context* _context);

private:
  bool LoadFont();
  bool LoadMenuElement();
  bool LoadInfoElement();
  bool LoadLogElement();
  bool LoadHookElement();

  // Release all loaded elements
  void ReleaseElements();

  // The context hosting the debug documents.
  Core::DocumentContext* host_context;
  // The context we're debugging.
  Core::DocumentContext* debug_context;

  // The debug elements.
  Core::ElementDocument* menu_element;
  ElementInfo* info_element;
  ElementLog* log_element;
  ElementContextHook* hook_element;
  SystemInterface* log_hook;

  bool render_outlines;

  // Keep track of instanced elements for leak tracking.
  typedef std::set<Core::Element*> ElementInstanceMap;
  ElementInstanceMap elements;

  Core::Context* m_context;
  // Singleton instance
  static Plugin** kPluginInstanceKey;
};
}  // namespace Debugger
}  // namespace Rocket

#endif
