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
// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "../../Include/Rocket/Core.h"
#include <algorithm>
#include "../../Include/Rocket/Core/Context.h"
#include "../../Include/Rocket/Core/DocumentContext.h"
#include "GeometryDatabase.h"
#include "PluginRegistry.h"
#include "StyleSheetFactory.h"
#include "TemplateCache.h"
#include "TextureDatabase.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

// Rocket's renderer interface.
static RenderInterface** kRenderInterfaceKey;
/// Rocket's system interface.
static SystemInterface** kSystemInterfaceKey;
// Rocket's file I/O interface.
FileInterface** kFileInterfaceKey;
static bool* kCoreInitializedKey;

typedef std::map<String, DocumentContext*> ContextMap;
static ContextMap* kContextsKey;

#ifndef ROCKET_VERSION
#define ROCKET_VERSION "custom"
#endif

void RegisterContextValues(Context* _context) {
  _context->RegisterCachedType(kRenderInterfaceKey);
  _context->RegisterCachedType(kSystemInterfaceKey);
  _context->RegisterCachedType(kFileInterfaceKey);
  _context->RegisterCachedType(kCoreInitializedKey);
  _context->RegisterCachedType(kContextsKey);
}

/**
        A 'plugin' for unobtrusively intercepting the destruction of contexts.
 */

class PluginContextRelease : public Plugin {
public:
  PluginContextRelease(Context* _context) : Plugin(_context) {}
  virtual void OnShutdown() {
    delete this;
  }

  virtual void OnContextDestroy(DocumentContext* context) {
    (*m_context)(kContextsKey).erase(context->GetName());
  }
};

bool Initialise(Context* _context) {
  auto& system_interface = (*_context)(kSystemInterfaceKey);
  auto& file_interface = (*_context)(kFileInterfaceKey);
  auto& initialised = (*_context)(kCoreInitializedKey);
  // Check for valid interfaces, or install default interfaces as appropriate.
  if (system_interface == NULL) {
    Log::Message(_context, Log::LT_ERROR, "No system interface set!");
    return false;
  }

  if (file_interface == NULL) {
    Log::Message(_context, Log::LT_ERROR, "No file interface set!");
    return false;
  }

  Log::Initialise(_context);

  TextureDatabase::Initialise(_context);

  FontDatabase::Initialise(_context);

  StyleSheetSpecification::Initialise(_context);
  StyleSheetFactory::Initialise(_context);

  TemplateCache::Initialise(_context);

  Factory::Initialise(_context);

  // Notify all plugins we're starting up.
  PluginRegistry::RegisterPlugin(_context, new PluginContextRelease(_context));
  PluginRegistry::NotifyInitialise(_context);

  initialised = true;

  return true;
}

void Shutdown(Context* _context) {
  auto& contexts = (*_context)(kContextsKey);
  auto& initialised = (*_context)(kCoreInitializedKey);
  auto& render_interface = (*_context)(kRenderInterfaceKey);
  auto& file_interface = (*_context)(kFileInterfaceKey);
  auto& system_interface = (*_context)(kSystemInterfaceKey);
  // Notify all plugins we're being shutdown.
  PluginRegistry::NotifyShutdown(_context);

  // Release all remaining contexts.
  for (ContextMap::iterator itr = contexts.begin(); itr != contexts.end();
       ++itr)
    Core::Log::Message(_context, Log::LT_WARNING,
        "DocumentContext '%s' still active on shutdown.",
        (*itr).first.CString());
  contexts.clear();

  TemplateCache::Shutdown(_context);
  StyleSheetFactory::Shutdown(_context);
  StyleSheetSpecification::Shutdown(_context);
  FontDatabase::Shutdown(_context);
  TextureDatabase::Shutdown(_context);
  Factory::Shutdown(_context);

  Log::Shutdown(_context);

  initialised = false;

  if (render_interface != NULL)
    render_interface->RemoveReference();

  if (file_interface != NULL)
    file_interface->RemoveReference();

  if (system_interface != NULL)
    system_interface->RemoveReference();

  render_interface = NULL;
  file_interface = NULL;
  system_interface = NULL;
}

// Returns the version of this Rocket library.
String GetVersion() {
  return ROCKET_VERSION;
}

// Sets the interface through which all Rocket messages will be routed.
void SetSystemInterface(Context* _context, SystemInterface* _system_interface) {
  auto& system_interface = (*_context)(kSystemInterfaceKey);
  if (system_interface == _system_interface)
    return;

  if (system_interface != NULL)
    system_interface->RemoveReference();

  system_interface = _system_interface;
  if (system_interface != NULL)
    system_interface->AddReference();
}

// Returns Rocket's system interface.
SystemInterface* GetSystemInterface(Context* _context) {
  auto& system_interface = (*_context)(kSystemInterfaceKey);
  return system_interface;
}

// Sets the interface through which all rendering requests are made.
void SetRenderInterface(Context* _context, RenderInterface* _render_interface) {
  auto& render_interface = (*_context)(kRenderInterfaceKey);

  if (render_interface == _render_interface)
    return;

  if (render_interface != NULL)
    render_interface->RemoveReference();

  render_interface = _render_interface;
  if (render_interface != NULL)
    render_interface->AddReference();
}

// Returns Rocket's render interface.
RenderInterface* GetRenderInterface(Context* _context) {
  auto& render_interface = (*_context)(kRenderInterfaceKey);

  return render_interface;
}

// Sets the interface through which all file I/O requests are made.
void SetFileInterface(Context* _context, FileInterface* _file_interface) {
  auto& file_interface = (*_context)(kFileInterfaceKey);

  if (file_interface == _file_interface)
    return;

  if (file_interface != NULL)
    file_interface->RemoveReference();

  file_interface = _file_interface;
  if (file_interface != NULL)
    file_interface->AddReference();
}

// Returns Rocket's file interface.
FileInterface* GetFileInterface(Context* _context) {
  auto& file_interface = (*_context)(kFileInterfaceKey);

  return file_interface;
}

// Creates a new element context.
DocumentContext* CreateDocumentContext(Context* _context, const String& name,
    const Vector2i& dimensions, RenderInterface* custom_render_interface) {
  auto& initialised = (*_context)(kCoreInitializedKey);
  auto& render_interface = (*_context)(kRenderInterfaceKey);
  auto& contexts = (*_context)(kContextsKey);
  if (!initialised)
    return NULL;

  if (custom_render_interface == NULL && render_interface == NULL) {
    Log::Message(_context, Log::LT_WARNING,
        "Failed to create context '%s', no render interface specified and no "
        "default render interface exists.",
        name.CString());
    return NULL;
  }

  if (GetContext(_context, name) != NULL) {
    Log::Message(_context, Log::LT_WARNING,
        "Failed to create context '%s', context already exists.",
        name.CString());
    return NULL;
  }

  DocumentContext* new_context = Factory::InstanceContext(_context, name);
  if (new_context == NULL) {
    Log::Message(_context, Log::LT_WARNING,
        "Failed to instance context '%s', instancer returned NULL.",
        name.CString());
    return NULL;
  }

  // Set the render interface on the context, and add a reference onto it.
  if (custom_render_interface)
    new_context->render_interface = custom_render_interface;
  else
    new_context->render_interface = render_interface;
  new_context->render_interface->AddReference();

  new_context->SetDimensions(dimensions);
  contexts[name] = new_context;

  PluginRegistry::NotifyContextCreate(_context, new_context);

  return new_context;
}

// Fetches a previously constructed context by name.
DocumentContext* GetContext(Context* _context, const String& name) {
  auto& contexts = (*_context)(kContextsKey);
  ContextMap::iterator i = contexts.find(name);
  if (i == contexts.end())
    return NULL;

  return (*i).second;
}

// Fetches a context by index.
DocumentContext* GetContext(Context* _context, int index) {
  auto& contexts = (*_context)(kContextsKey);
  ContextMap::iterator i = contexts.begin();
  int count = 0;

  if (index >= GetNumContexts(_context))
    index = GetNumContexts(_context) - 1;

  while (count < index) {
    ++i;
    ++count;
  }

  if (i == contexts.end())
    return NULL;

  return (*i).second;
}

// Returns the number of active contexts.
int GetNumContexts(Context* _context) {
  auto& contexts = (*_context)(kContextsKey);
  return (int)contexts.size();
}

// Registers a generic rocket plugin
void RegisterPlugin(Context* _context, Plugin* plugin) {
  auto& initialised = (*_context)(kCoreInitializedKey);
  if (initialised)
    plugin->OnInitialise();

  PluginRegistry::RegisterPlugin(_context, plugin);
}

// Forces all compiled geometry handles generated by libRocket to be released.
void ReleaseCompiledGeometries(Context*) {
  GeometryDatabase::ReleaseGeometries();
}

// Forces all texture handles loaded and generated by libRocket to be released.
void ReleaseTextures(Context* _context) {
  TextureDatabase::ReleaseTextures(_context);
}
}  // namespace Core
}  // namespace Rocket
