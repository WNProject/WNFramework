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

// clang-format off

#include "../../Include/Rocket/Core/StyleSheetSpecification.h"
#include "PropertyParserColour.h"
#include "PropertyParserKeyword.h"
#include "PropertyParserNumber.h"
#include "PropertyParserString.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

static StyleSheetSpecification** kStyleSheetSpecificationKey;

void StyleSheetSpecification::RegisterContextValues(Context* _context) {
  _context->RegisterCachedType(kStyleSheetSpecificationKey);
}

StyleSheetSpecification::StyleSheetSpecification(Context* _context)
  : properties(_context), m_context(_context) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  ROCKET_ASSERT(instance == NULL);
  instance = this;
}

StyleSheetSpecification::~StyleSheetSpecification() {
  auto& instance = (*m_context)(kStyleSheetSpecificationKey);
  ROCKET_ASSERT(instance == this);
  instance = NULL;
}

bool StyleSheetSpecification::Initialise(Context* _context) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  if (instance == NULL) {
    new StyleSheetSpecification(_context);

    instance->RegisterDefaultParsers();
    instance->RegisterDefaultProperties();
  }

  return true;
}

void StyleSheetSpecification::Shutdown(Context* _context) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  if (instance != NULL) {
    for (ParserMap::iterator iterator = instance->parsers.begin();
         iterator != instance->parsers.end(); iterator++)
      (*iterator).second->Release();

    delete instance;
  }
}

// Registers a parser for use in property definitions.
bool StyleSheetSpecification::RegisterParser(
    Context* _context, const String& parser_name, PropertyParser* parser) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  ParserMap::iterator iterator = instance->parsers.find(parser_name);
  if (iterator != instance->parsers.end())
    (*iterator).second->Release();

  instance->parsers[parser_name] = parser;
  return true;
}

// Returns the parser registered with a specific name.
PropertyParser* StyleSheetSpecification::GetParser(
    Context* _context, const String& parser_name) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  ParserMap::iterator iterator = instance->parsers.find(parser_name);
  if (iterator == instance->parsers.end())
    return NULL;

  return (*iterator).second;
}

// Registers a property with a new definition.
PropertyDefinition& StyleSheetSpecification::RegisterProperty(Context* _context,
    const String& property_name, const String& default_value, bool inherited,
    bool forces_layout) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  return instance->properties.RegisterProperty(
      property_name, default_value, inherited, forces_layout);
}

// Returns a property definition.
const PropertyDefinition* StyleSheetSpecification::GetProperty(
    Context* _context, const String& property_name) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  return instance->properties.GetProperty(property_name);
}

// Fetches a list of the names of all registered property definitions.
const PropertyNameList& StyleSheetSpecification::GetRegisteredProperties(
    Context* _context) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  return instance->properties.GetRegisteredProperties();
}

const PropertyNameList&
StyleSheetSpecification::GetRegisteredInheritedProperties(Context* _context) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  return instance->properties.GetRegisteredInheritedProperties();
}

// Registers a shorthand property definition.
bool StyleSheetSpecification::RegisterShorthand(Context* _context,
    const String& shorthand_name, const String& property_names,
    PropertySpecification::ShorthandType type) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  return instance->properties.RegisterShorthand(
      shorthand_name, property_names, type);
}

// Returns a shorthand definition.
const PropertyShorthandDefinition* StyleSheetSpecification::GetShorthand(
    Context* _context, const String& shorthand_name) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  return instance->properties.GetShorthand(shorthand_name);
}

// Parses a property declaration, setting any parsed and validated properties on
// the given dictionary.
bool StyleSheetSpecification::ParsePropertyDeclaration(Context* _context,
    PropertyDictionary& dictionary, const String& property_name,
    const String& property_value, const String& source_file,
    int source_line_number) {
  auto& instance = (*_context)(kStyleSheetSpecificationKey);
  return instance->properties.ParsePropertyDeclaration(dictionary,
      property_name, property_value, source_file, source_line_number);
}

// Registers Rocket's default parsers.
void StyleSheetSpecification::RegisterDefaultParsers() {
  RegisterParser(m_context, "number", new PropertyParserNumber());
  RegisterParser(m_context, "keyword", new PropertyParserKeyword());
  RegisterParser(m_context, "string", new PropertyParserString());
  RegisterParser(m_context, COLOR, new PropertyParserColour());
}

// Registers Rocket's default style properties.
void StyleSheetSpecification::RegisterDefaultProperties() {
  // Style property specifications (ala RCSS).

  RegisterProperty(m_context, MARGIN_TOP, "0px", false, true)
      .AddParser("keyword", "auto")
      .AddParser("number");
  RegisterProperty(m_context, MARGIN_RIGHT, "0px", false, true)
      .AddParser("keyword", "auto")
      .AddParser("number");
  RegisterProperty(m_context, MARGIN_BOTTOM, "0px", false, true)
      .AddParser("keyword", "auto")
      .AddParser("number");
  RegisterProperty(m_context, MARGIN_LEFT, "0px", false, true)
      .AddParser("keyword", "auto")
      .AddParser("number");
  RegisterShorthand(m_context, MARGIN,
      "margin-top, margin-right, margin-bottom, margin-left");

  RegisterProperty(m_context, PADDING_TOP, "0px", false, true)
      .AddParser("number");
  RegisterProperty(m_context, PADDING_RIGHT, "0px", false, true)
      .AddParser("number");
  RegisterProperty(m_context, PADDING_BOTTOM, "0px", false, true)
      .AddParser("number");
  RegisterProperty(m_context, PADDING_LEFT, "0px", false, true)
      .AddParser("number");
  RegisterShorthand(m_context, PADDING,
      "padding-top, padding-right, padding-bottom, padding-left");

  RegisterProperty(m_context, BORDER_TOP_WIDTH, "0px", false, true)
      .AddParser("number");
  RegisterProperty(m_context, BORDER_RIGHT_WIDTH, "0px", false, true)
      .AddParser("number");
  RegisterProperty(m_context, BORDER_BOTTOM_WIDTH, "0px", false, true)
      .AddParser("number");
  RegisterProperty(m_context, BORDER_LEFT_WIDTH, "0px", false, true)
      .AddParser("number");
  RegisterShorthand(m_context, BORDER_WIDTH,
      "border-top-width, border-right-width, border-bottom-width, "
      "border-left-width");

  RegisterProperty(m_context, BORDER_TOP_COLOR, "black", false, false)
      .AddParser(COLOR);
  RegisterProperty(m_context, BORDER_RIGHT_COLOR, "black", false, false)
      .AddParser(COLOR);
  RegisterProperty(m_context, BORDER_BOTTOM_COLOR, "black", false, false)
      .AddParser(COLOR);
  RegisterProperty(m_context, BORDER_LEFT_COLOR, "black", false, false)
      .AddParser(COLOR);
  RegisterShorthand(m_context, BORDER_COLOR,
      "border-top-color, border-right-color, border-bottom-color, "
      "border-left-color");

  RegisterShorthand(
      m_context, BORDER_TOP, "border-top-width, border-top-color");
  RegisterShorthand(
      m_context, BORDER_RIGHT, "border-right-width, border-right-color");
  RegisterShorthand(
      m_context, BORDER_BOTTOM, "border-bottom-width, border-bottom-color");
  RegisterShorthand(
      m_context, BORDER_LEFT, "border-left-width, border-left-color");

  RegisterProperty(m_context, DISPLAY, "inline", false, true)
      .AddParser("keyword", "none, block, inline, inline-block");
  RegisterProperty(m_context, POSITION, "static", false, true)
      .AddParser("keyword", "static, relative, absolute, fixed");
  RegisterProperty(m_context, TOP, "auto", false, false)
      .AddParser("keyword", "auto")
      .AddParser("number");
  RegisterProperty(m_context, RIGHT, "auto", false, false)
      .AddParser("keyword", "auto")
      .AddParser("number");
  RegisterProperty(m_context, BOTTOM, "auto", false, false)
      .AddParser("keyword", "auto")
      .AddParser("number");
  RegisterProperty(m_context, LEFT, "auto", false, false)
      .AddParser("keyword", "auto")
      .AddParser("number");

  RegisterProperty(m_context, FLOAT, "none", false, true)
      .AddParser("keyword", "none, left, right");
  RegisterProperty(m_context, CLEAR, "none", false, true)
      .AddParser("keyword", "none, left, right, both");

  RegisterProperty(m_context, Z_INDEX, "auto", false, false)
      .AddParser("keyword", "auto, top, bottom")
      .AddParser("number");

  RegisterProperty(m_context, WIDTH, "auto", false, true)
      .AddParser("keyword", "auto")
      .AddParser("number");
  RegisterProperty(m_context, MIN_WIDTH, "0px", false, true)
      .AddParser("number");
  RegisterProperty(m_context, MAX_WIDTH, "-1", false, true).AddParser("number");

  RegisterProperty(m_context, HEIGHT, "auto", false, true)
      .AddParser("keyword", "auto")
      .AddParser("number");
  RegisterProperty(m_context, MIN_HEIGHT, "0px", false, true)
      .AddParser("number");
  RegisterProperty(m_context, MAX_HEIGHT, "-1", false, true)
      .AddParser("number");

  RegisterProperty(m_context, LINE_HEIGHT, "1.2", true, true)
      .AddParser("number");
  RegisterProperty(m_context, VERTICAL_ALIGN, "baseline", false, true)
      .AddParser("keyword",
          "baseline, middle, sub, super, text-top, text-bottom, top, bottom")
      .AddParser("number");

  RegisterProperty(m_context, OVERFLOW_X, "visible", false, true)
      .AddParser("keyword", "visible, hidden, auto, scroll");
  RegisterProperty(m_context, OVERFLOW_Y, "visible", false, true)
      .AddParser("keyword", "visible, hidden, auto, scroll");
  RegisterShorthand(m_context, "overflow", "overflow-x, overflow-y",
      PropertySpecification::REPLICATE);
  RegisterProperty(m_context, CLIP, "auto", true, false)
      .AddParser("keyword", "auto, none")
      .AddParser("number");
  RegisterProperty(m_context, VISIBILITY, "visible", false, false)
      .AddParser("keyword", "visible, hidden");

  // Need some work on this if we are to include images.
  RegisterProperty(m_context, BACKGROUND_COLOR, "transparent", false, false)
      .AddParser(COLOR);
  RegisterShorthand(m_context, BACKGROUND, BACKGROUND_COLOR);

  RegisterProperty(m_context, COLOR, "white", true, false).AddParser(COLOR);

  RegisterProperty(m_context, FONT_FAMILY, "", true, true).AddParser("string");
  RegisterProperty(m_context, FONT_CHARSET, "U+0020-007E", true, false)
      .AddParser("string");
  RegisterProperty(m_context, FONT_STYLE, "normal", true, true)
      .AddParser("keyword", "normal, italic");
  RegisterProperty(m_context, FONT_WEIGHT, "normal", true, true)
      .AddParser("keyword", "normal, bold");
  RegisterProperty(m_context, FONT_SIZE, "12", true, true).AddParser("number");
  RegisterShorthand(m_context, FONT,
      "font-style, font-weight, font-size, font-family, font-charset");

  RegisterProperty(m_context, TEXT_ALIGN, LEFT, true, true)
      .AddParser("keyword", "left, right, center, justify");
  RegisterProperty(m_context, TEXT_DECORATION, "none", true, false)
      .AddParser("keyword",
          "none, underline" /*"none, underline, overline, line-through"*/);
  RegisterProperty(m_context, TEXT_TRANSFORM, "none", true, true)
      .AddParser("keyword", "none, capitalize, uppercase, lowercase");
  RegisterProperty(m_context, WHITE_SPACE, "normal", true, true)
      .AddParser("keyword", "normal, pre, nowrap, pre-wrap, pre-line");

  RegisterProperty(m_context, CURSOR, "auto", true, false)
      .AddParser("keyword", "auto")
      .AddParser("string");

  // Functional property specifications.
  RegisterProperty(m_context, DRAG, "none", false, false)
      .AddParser("keyword", "none, drag, drag-drop, block, clone");
  RegisterProperty(m_context, TAB_INDEX, "none", false, false)
      .AddParser("keyword", "none, auto");
  RegisterProperty(m_context, FOCUS, "auto", true, false)
      .AddParser("keyword", "none, auto");

  RegisterProperty(m_context, SCROLLBAR_MARGIN, "0", false, false)
      .AddParser("number");
  RegisterProperty(m_context, OPACITY, "1", true, false).AddParser("number");
  RegisterProperty(m_context, COLOR_MULTIPLIER, "white", true, false)
      .AddParser(COLOR);
}
}  // namespace Core
}  // namespace Rocket
