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
#include "PluginRegistry.h"
#include "../../Include/Rocket/Core/Context.h"
#include "../../Include/Rocket/Core/Plugin.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

typedef std::vector<Plugin*> PluginList;
static PluginList* kBasicKey;
static PluginList* kDocumentKey;
static PluginList* kElementKey;

void PluginRegistry::RegisterContextValues(Context* _context) {
  _context->RegisterCachedType(kBasicKey);
  _context->RegisterCachedType(kDocumentKey);
  _context->RegisterCachedType(kElementKey);
}

PluginRegistry::PluginRegistry() {}

void PluginRegistry::RegisterPlugin(Core::Context* _context, Plugin* plugin) {
  auto& basic_plugins = (*_context)(kBasicKey);
  auto& document_plugins = (*_context)(kDocumentKey);
  auto& element_plugins = (*_context)(kElementKey);
  int event_classes = plugin->GetEventClasses();

  if (event_classes & Plugin::EVT_BASIC)
    basic_plugins.push_back(plugin);
  if (event_classes & Plugin::EVT_DOCUMENT)
    document_plugins.push_back(plugin);
  if (event_classes & Plugin::EVT_ELEMENT)
    element_plugins.push_back(plugin);
}

// Calls OnInitialise() on all plugins.
void PluginRegistry::NotifyInitialise(Core::Context* _context) {
  auto& basic_plugins = (*_context)(kBasicKey);
  for (size_t i = 0; i < basic_plugins.size(); ++i)
    basic_plugins[i]->OnInitialise();
}

// Calls OnShutdown() on all plugins.
void PluginRegistry::NotifyShutdown(Core::Context* _context) {
  auto& basic_plugins = (*_context)(kBasicKey);
  auto& document_plugins = (*_context)(kDocumentKey);
  auto& element_plugins = (*_context)(kElementKey);
  while (!basic_plugins.empty()) {
    basic_plugins.back()->OnShutdown();
    basic_plugins.pop_back();
  }
  document_plugins.clear();
  element_plugins.clear();
}

// Calls OnContextCreate() on all plugins.
void PluginRegistry::NotifyContextCreate(
    Core::Context* _context, DocumentContext* context) {
  auto& basic_plugins = (*_context)(kBasicKey);
  for (size_t i = 0; i < basic_plugins.size(); ++i)
    basic_plugins[i]->OnContextCreate(context);
}

// Calls OnContextDestroy() on all plugins.
void PluginRegistry::NotifyContextDestroy(
    Core::Context* _context, DocumentContext* context) {
  auto& basic_plugins = (*_context)(kBasicKey);
  for (size_t i = 0; i < basic_plugins.size(); ++i)
    basic_plugins[i]->OnContextDestroy(context);
}

// Calls OnDocumentOpen() on all plugins.
void PluginRegistry::NotifyDocumentOpen(Core::Context* _context,
    DocumentContext* context, const String& document_path) {
  auto& document_plugins = (*_context)(kDocumentKey);
  for (size_t i = 0; i < document_plugins.size(); ++i)
    document_plugins[i]->OnDocumentOpen(context, document_path);
}

// Calls OnDocumentLoad() on all plugins.
void PluginRegistry::NotifyDocumentLoad(
    Core::Context* _context, ElementDocument* document) {
  auto& document_plugins = (*_context)(kDocumentKey);
  for (size_t i = 0; i < document_plugins.size(); ++i)
    document_plugins[i]->OnDocumentLoad(document);
}

// Calls OnDocumentUnload() on all plugins.
void PluginRegistry::NotifyDocumentUnload(
    Core::Context* _context, ElementDocument* document) {
  auto& document_plugins = (*_context)(kDocumentKey);
  for (size_t i = 0; i < document_plugins.size(); ++i)
    document_plugins[i]->OnDocumentUnload(document);
}

// Calls OnElementCreate() on all plugins.
void PluginRegistry::NotifyElementCreate(
    Core::Context* _context, Element* element) {
  auto& element_plugins = (*_context)(kElementKey);
  for (size_t i = 0; i < element_plugins.size(); ++i)
    element_plugins[i]->OnElementCreate(element);
}

// Calls OnElementDestroy() on all plugins.
void PluginRegistry::NotifyElementDestroy(
    Core::Context* _context, Element* element) {
  auto& element_plugins = (*_context)(kElementKey);
  for (size_t i = 0; i < element_plugins.size(); ++i)
    element_plugins[i]->OnElementDestroy(element);
}
}  // namespace Core
}  // namespace Rocket
