// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../../Include/Rocket/Core/Context.h"
#include "../../Include/Rocket/Controls/Clipboard.h"
#include "../../Include/Rocket/Controls/Controls.h"
#include "../../Include/Rocket/Controls/DataSource.h"
#include "../../Include/Rocket/Core/BitmapFont/FontProvider.h"
#include "../../Include/Rocket/Core/Core.h"
#include "../../Include/Rocket/Core/Factory.h"
#include "../../Include/Rocket/Core/FontDatabase.h"
#include "../../Include/Rocket/Core/Geometry.h"
#include "../../Include/Rocket/Core/ReferenceCountable.h"
#include "../../Include/Rocket/Core/StyleSheetSpecification.h"
#include "../../Include/Rocket/Core/TTFFont/FontProvider.h"
#include "../../Include/Rocket/Core/XMLParser.h"
#include "../Debugger/Geometry.h"
#include "../Debugger/Plugin.h"
#include "ElementStyle.h"
#include "LayoutEngine.h"
#include "PluginRegistry.h"
#include "StyleSheetFactory.h"
#include "TemplateCache.h"
#include "TextureDatabase.h"

namespace Rocket {
namespace Core {

Context::Context() {
  Controls::Clipboard::RegisterContextValues(this);
  Controls::RegisterContextValues(this);
  Controls::DataSource::RegisterContextValues(this);
  BitmapFont::FontProvider::RegisterContextValues(this);
  TTFFont::FontProvider::RegisterContextValues(this);
  Core::RegisterContextValues(this);
  FontDatabase::RegisterContextValues(this);
  Factory::RegisterContextValues(this);
  Geometry::RegisterContextValues(this);
  ElementStyle::RegisterContextValues(this);
  LayoutEngine::RegisterContextValues(this);
  PluginRegistry::RegisterContextValues(this);
  StyleSheetFactory::RegisterContextValues(this);
  StyleSheetSpecification::RegisterContextValues(this);
  TemplateCache::RegisterContextValues(this);
  TextureDatabase::RegisterContextValues(this);
  XMLParser::RegisterContextValues(this);
  Debugger::Geometry::RegisterContextValues(this);
  Debugger::Plugin::RegisterContextValues(this);
}

}  // namespace Core
}  // namespace Rocket