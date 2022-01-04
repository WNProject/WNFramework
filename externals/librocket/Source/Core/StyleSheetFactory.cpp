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
#include "StyleSheetFactory.h"
#include "../../Include/Rocket/Core/Context.h"
#include "../../Include/Rocket/Core/Log.h"
#include "../../Include/Rocket/Core/StyleSheet.h"
#include "StreamFile.h"
#include "StyleSheetNodeSelectorEmpty.h"
#include "StyleSheetNodeSelectorFirstChild.h"
#include "StyleSheetNodeSelectorFirstOfType.h"
#include "StyleSheetNodeSelectorLastChild.h"
#include "StyleSheetNodeSelectorLastOfType.h"
#include "StyleSheetNodeSelectorNthChild.h"
#include "StyleSheetNodeSelectorNthLastChild.h"
#include "StyleSheetNodeSelectorNthLastOfType.h"
#include "StyleSheetNodeSelectorNthOfType.h"
#include "StyleSheetNodeSelectorOnlyChild.h"
#include "StyleSheetNodeSelectorOnlyOfType.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

static StyleSheetFactory** kStyleSheetInstanceKey;

void StyleSheetFactory::RegisterContextValues(Context* _context) {
  _context->RegisterCachedType(kStyleSheetInstanceKey);
}

StyleSheetFactory::StyleSheetFactory(Context* _context) : m_context(_context) {
  auto& instance = (*_context)(kStyleSheetInstanceKey);
  ROCKET_ASSERT(instance == NULL);
  instance = this;
}

StyleSheetFactory::~StyleSheetFactory() {
  auto& instance = (*m_context)(kStyleSheetInstanceKey);
  instance = NULL;
}

bool StyleSheetFactory::Initialise(Core::Context* _context) {
  new StyleSheetFactory(_context);
  auto& instance = (*_context)(kStyleSheetInstanceKey);

  instance->selectors["nth-child"] = new StyleSheetNodeSelectorNthChild();
  instance->selectors["nth-last-child"] =
      new StyleSheetNodeSelectorNthLastChild();
  instance->selectors["nth-of-type"] = new StyleSheetNodeSelectorNthOfType();
  instance->selectors["nth-last-of-type"] =
      new StyleSheetNodeSelectorNthLastOfType();
  instance->selectors["first-child"] = new StyleSheetNodeSelectorFirstChild();
  instance->selectors["last-child"] = new StyleSheetNodeSelectorLastChild();
  instance->selectors["first-of-type"] =
      new StyleSheetNodeSelectorFirstOfType();
  instance->selectors["last-of-type"] = new StyleSheetNodeSelectorLastOfType();
  instance->selectors["only-child"] = new StyleSheetNodeSelectorOnlyChild();
  instance->selectors["only-of-type"] = new StyleSheetNodeSelectorOnlyOfType();
  instance->selectors["empty"] = new StyleSheetNodeSelectorEmpty();

  return true;
}

void StyleSheetFactory::Shutdown(Core::Context* _context) {
  auto& instance = (*_context)(kStyleSheetInstanceKey);
  if (instance != NULL) {
    ClearStyleSheetCache(_context);

    for (SelectorMap::iterator i = instance->selectors.begin();
         i != instance->selectors.end(); ++i)
      delete (*i).second;

    delete instance;
  }
}

StyleSheet* StyleSheetFactory::GetStyleSheet(
    Core::Context* _context, const String& sheet_name) {
  // Look up the sheet definition in the cache
  auto& instance = (*_context)(kStyleSheetInstanceKey);
  StyleSheets::iterator itr = instance->stylesheets.find(sheet_name);
  if (itr != instance->stylesheets.end()) {
    (*itr).second->AddReference();
    return (*itr).second;
  }

  // Don't currently have the sheet, attempt to load it
  StyleSheet* sheet = instance->LoadStyleSheet(sheet_name);
  if (sheet == NULL)
    return NULL;

  // Add it to the cache, and add a reference count so the cache will keep hold
  // of it.
  instance->stylesheets[sheet_name] = sheet;
  sheet->AddReference();

  return sheet;
}

StyleSheet* StyleSheetFactory::GetStyleSheet(
    Context* _context, const StringList& sheets) {
  auto& instance = (*_context)(kStyleSheetInstanceKey);
  // Generate a unique key for these sheets
  String combined_key;
  for (size_t i = 0; i < sheets.size(); i++) {
    URL path(sheets[i]);
    combined_key += path.GetFileName();
  }

  // Look up the sheet definition in the cache.
  StyleSheets::iterator itr = instance->stylesheet_cache.find(combined_key);
  if (itr != instance->stylesheet_cache.end()) {
    (*itr).second->AddReference();
    return (*itr).second;
  }

  // Load and combine the sheets.
  StyleSheet* sheet = NULL;
  for (size_t i = 0; i < sheets.size(); i++) {
    StyleSheet* sub_sheet = GetStyleSheet(_context, sheets[i]);
    if (sub_sheet) {
      if (sheet) {
        StyleSheet* new_sheet = sheet->CombineStyleSheet(sub_sheet);
        sheet->RemoveReference();
        sub_sheet->RemoveReference();

        sheet = new_sheet;
      } else
        sheet = sub_sheet;
    } else
      Log::Message(_context, Log::LT_ERROR, "Failed to load style sheet %s.",
          sheets[i].CString());
  }

  if (sheet == NULL)
    return NULL;

  // Add to cache, and a reference to the sheet to hold it in the cache.
  instance->stylesheet_cache[combined_key] = sheet;
  sheet->AddReference();
  return sheet;
}

// Clear the style sheet cache.
void StyleSheetFactory::ClearStyleSheetCache(Context* _context) {
  auto& instance = (*_context)(kStyleSheetInstanceKey);
  for (StyleSheets::iterator i = instance->stylesheets.begin();
       i != instance->stylesheets.end(); ++i)
    (*i).second->RemoveReference();

  for (StyleSheets::iterator i = instance->stylesheet_cache.begin();
       i != instance->stylesheet_cache.end(); ++i)
    (*i).second->RemoveReference();

  instance->stylesheets.clear();
  instance->stylesheet_cache.clear();
}

// Returns one of the available node selectors.
StyleSheetNodeSelector* StyleSheetFactory::GetSelector(
    Context* _context, const String& name) {
  auto& instance = (*_context)(kStyleSheetInstanceKey);
  size_t index = name.Find("(");
  SelectorMap::iterator i = instance->selectors.find(name.Substring(0, index));
  if (i == instance->selectors.end())
    return NULL;
  return (*i).second;
}

StyleSheet* StyleSheetFactory::LoadStyleSheet(const String& sheet) {
  StyleSheet* new_style_sheet = NULL;

  // Open stream, construct new sheet and pass the stream into the sheet
  // TODO: Make this support ASYNC
  StreamFile* stream = new StreamFile(m_context);
  if (stream->Open(sheet)) {
    new_style_sheet = new StyleSheet(m_context);
    if (!new_style_sheet->LoadStyleSheet(stream)) {
      new_style_sheet->RemoveReference();
      new_style_sheet = NULL;
    }
  }

  stream->RemoveReference();
  return new_style_sheet;
}
}  // namespace Core
}  // namespace Rocket
