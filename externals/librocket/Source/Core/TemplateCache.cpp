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
#include "TemplateCache.h"
#include "../../Include/Rocket/Core/Log.h"
#include "StreamFile.h"
#include "Template.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

static TemplateCache** kTemplateInstance;

void TemplateCache::RegisterContextValues(Context* _context) {
  _context->RegisterCachedType(kTemplateInstance);
}

TemplateCache::TemplateCache(Context* _context) : m_context(_context) {
  auto& instance = (*m_context)(kTemplateInstance);
  (void)instance;
  ROCKET_ASSERT(instance == NULL);
  instance = this;
}

TemplateCache::~TemplateCache() {
  auto& instance = (*m_context)(kTemplateInstance);
  for (Templates::iterator itr = instance->templates.begin();
       itr != instance->templates.end(); ++itr) {
    delete (*itr).second;
  }

  instance = NULL;
}

bool TemplateCache::Initialise(Context* _context) {
  new TemplateCache(_context);

  return true;
}

void TemplateCache::Shutdown(Context* _context) {
  auto* instance = (*_context)(kTemplateInstance);
  delete instance;
}

Template* TemplateCache::LoadTemplate(Context* _context, const String& name) {
  auto* instance = (*_context)(kTemplateInstance);
  // Check if the template is already loaded
  Templates::iterator itr = instance->templates.find(name);
  if (itr != instance->templates.end())
    return (*itr).second;

  // Nope, we better load it
  Template* new_template = NULL;
  StreamFile* stream = new StreamFile(_context);
  if (stream->Open(name)) {
    new_template = new Template(_context);
    if (!new_template->Load(stream)) {
      Log::Message(_context, Log::LT_ERROR, "Failed to load template %s.",
          name.CString());
      delete new_template;
      new_template = NULL;
    } else if (new_template->GetName().Empty()) {
      Log::Message(_context, Log::LT_ERROR,
          "Failed to load template %s, template is missing its name.",
          name.CString());
      delete new_template;
      new_template = NULL;
    } else {
      instance->templates[name] = new_template;
      instance->template_ids[new_template->GetName()] = new_template;
    }
  } else {
    Log::Message(_context, Log::LT_ERROR, "Failed to open template file %s.",
        name.CString());
  }
  stream->RemoveReference();

  return new_template;
}

Template* TemplateCache::GetTemplate(Context* _context, const String& name) {
  auto* instance = (*_context)(kTemplateInstance);
  // Check if the template is already loaded
  Templates::iterator itr = instance->template_ids.find(name);
  if (itr != instance->template_ids.end())
    return (*itr).second;

  return NULL;
}

void TemplateCache::Clear(Context* _context) {
  auto* instance = (*_context)(kTemplateInstance);
  for (Templates::iterator i = instance->templates.begin();
       i != instance->templates.end(); ++i)
    delete (*i).second;

  instance->templates.clear();
  instance->template_ids.clear();
}
}  // namespace Core
}  // namespace Rocket
