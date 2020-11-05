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
// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#include "../../Include/Rocket/Core.h"
#include "../../Include/Rocket/Core/Context.h"
#include "../../Include/Rocket/Core/StreamMemory.h"
#include "DecoratorNoneInstancer.h"
#include "DecoratorTiledBoxInstancer.h"
#include "DecoratorTiledHorizontalInstancer.h"
#include "DecoratorTiledImageInstancer.h"
#include "DecoratorTiledVerticalInstancer.h"
#include "DocumentContextInstancerDefault.h"
#include "ElementHandle.h"
#include "ElementImage.h"
#include "ElementTextDefault.h"
#include "EventInstancerDefault.h"
#include "FontEffectNoneInstancer.h"
#include "FontEffectOutlineInstancer.h"
#include "FontEffectShadowInstancer.h"
#include "PluginRegistry.h"
#include "PropertyParserColour.h"
#include "StreamFile.h"
#include "StyleSheetFactory.h"
#include "TemplateCache.h"
#include "XMLNodeHandlerBody.h"
#include "XMLNodeHandlerDefault.h"
#include "XMLNodeHandlerHead.h"
#include "XMLNodeHandlerTemplate.h"
#include "XMLParseTools.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

// Element instancers.
typedef std::map<String, ElementInstancer*> ElementInstancerMap;
static ElementInstancerMap* kElementKey;

// Decorator instancers.
typedef std::map<String, DecoratorInstancer*> DecoratorInstancerMap;
static DecoratorInstancerMap* kDecoratorKey;

// Font effect instancers.
typedef std::map<String, FontEffectInstancer*> FontEffectInstancerMap;
static FontEffectInstancerMap* kFontKey;

// The context instancer.
static DocumentContextInstancer** kContextKey;

// The event instancer
static EventInstancer** kEventKey;

// Event listener instancer.
static EventListenerInstancer** kListenerKey;

void Factory::RegisterContextValues(Context* _context) {
  _context->RegisterCachedType(kElementKey);
  _context->RegisterCachedType(kDecoratorKey);
  _context->RegisterCachedType(kFontKey);
  _context->RegisterCachedType(kContextKey);
  _context->RegisterCachedType(kEventKey);
  _context->RegisterCachedType(kListenerKey);
}

Factory::Factory() {}

Factory::~Factory() {}

bool Factory::Initialise(Context* _context) {
  auto& context_instancer = (*_context)(kContextKey);
  auto& event_instancer = (*_context)(kEventKey);
  auto& event_listener_instancer = (*_context)(kListenerKey);

  // Bind the default context instancer.
  if (context_instancer == NULL)
    context_instancer = new DocumentContextInstancerDefault(_context);

  // Bind default event instancer
  if (event_instancer == NULL)
    event_instancer = new EventInstancerDefault();

  // No default event listener instancer
  if (event_listener_instancer == NULL)
    event_listener_instancer = NULL;

  // Bind the default element instancers
  RegisterElementInstancer(
      _context, "*", new ElementInstancerGeneric<Element>())
      ->RemoveReference();
  RegisterElementInstancer(
      _context, "img", new ElementInstancerGeneric<ElementImage>())
      ->RemoveReference();
  RegisterElementInstancer(
      _context, "#text", new ElementInstancerGeneric<ElementTextDefault>())
      ->RemoveReference();
  RegisterElementInstancer(
      _context, "handle", new ElementInstancerGeneric<ElementHandle>())
      ->RemoveReference();
  RegisterElementInstancer(
      _context, "body", new ElementInstancerGeneric<ElementDocument>())
      ->RemoveReference();

  // Bind the default decorator instancers
  RegisterDecoratorInstancer(_context, "tiled-horizontal",
      new DecoratorTiledHorizontalInstancer(_context))
      ->RemoveReference();
  RegisterDecoratorInstancer(
      _context, "tiled-vertical", new DecoratorTiledVerticalInstancer(_context))
      ->RemoveReference();
  RegisterDecoratorInstancer(
      _context, "tiled-box", new DecoratorTiledBoxInstancer(_context))
      ->RemoveReference();
  RegisterDecoratorInstancer(
      _context, "image", new DecoratorTiledImageInstancer(_context))
      ->RemoveReference();
  RegisterDecoratorInstancer(
      _context, "none", new DecoratorNoneInstancer(_context))
      ->RemoveReference();

  RegisterFontEffectInstancer(
      _context, "shadow", new FontEffectShadowInstancer(_context))
      ->RemoveReference();
  RegisterFontEffectInstancer(
      _context, "outline", new FontEffectOutlineInstancer(_context))
      ->RemoveReference();
  RegisterFontEffectInstancer(
      _context, "none", new FontEffectNoneInstancer(_context))
      ->RemoveReference();

  // Register the core XML node handlers.
  XMLParser::RegisterNodeHandler(
      _context, "", new XMLNodeHandlerDefault(_context))
      ->RemoveReference();
  XMLParser::RegisterNodeHandler(
      _context, "body", new XMLNodeHandlerBody(_context))
      ->RemoveReference();
  XMLParser::RegisterNodeHandler(
      _context, "head", new XMLNodeHandlerHead(_context))
      ->RemoveReference();
  XMLParser::RegisterNodeHandler(
      _context, "template", new XMLNodeHandlerTemplate(_context))
      ->RemoveReference();

  return true;
}

void Factory::Shutdown(Context* _context) {
  auto& context_instancer = (*_context)(kContextKey);
  auto& event_instancer = (*_context)(kEventKey);
  auto& event_listener_instancer = (*_context)(kListenerKey);
  auto& element_instancers = (*_context)(kElementKey);
  auto& decorator_instancers = (*_context)(kDecoratorKey);
  auto& font_effect_instancers = (*_context)(kFontKey);

  for (ElementInstancerMap::iterator i = element_instancers.begin();
       i != element_instancers.end(); ++i)
    (*i).second->RemoveReference();
  element_instancers.clear();

  for (DecoratorInstancerMap::iterator i = decorator_instancers.begin();
       i != decorator_instancers.end(); ++i)
    (*i).second->RemoveReference();
  decorator_instancers.clear();

  for (FontEffectInstancerMap::iterator i = font_effect_instancers.begin();
       i != font_effect_instancers.end(); ++i)
    (*i).second->RemoveReference();
  font_effect_instancers.clear();

  if (context_instancer)
    context_instancer->RemoveReference();
  context_instancer = NULL;

  if (event_listener_instancer)
    event_listener_instancer->RemoveReference();
  event_listener_instancer = NULL;

  if (event_instancer)
    event_instancer->RemoveReference();
  event_instancer = NULL;

  XMLParser::ReleaseHandlers(_context);
}

// Registers the instancer to use when instancing contexts.
DocumentContextInstancer* Factory::RegisterContextInstancer(
    Context* _context, DocumentContextInstancer* instancer) {
  auto& context_instancer = (*_context)(kContextKey);
  instancer->AddReference();
  if (context_instancer != NULL)
    context_instancer->RemoveReference();

  context_instancer = instancer;
  return instancer;
}

// Instances a new context.
DocumentContext* Factory::InstanceContext(
    Context* _context, const String& name) {
  auto& context_instancer = (*_context)(kContextKey);
  DocumentContext* new_context = context_instancer->InstanceContext(name);
  if (new_context != NULL)
    new_context->SetInstancer(context_instancer);

  return new_context;
}

ElementInstancer* Factory::RegisterElementInstancer(
    Context* _context, const String& name, ElementInstancer* instancer) {
  auto& element_instancers = (*_context)(kElementKey);
  String lower_case_name = name.ToLower();
  instancer->AddReference();

  // Check if an instancer for this tag is already defined, if so release it
  ElementInstancerMap::iterator itr = element_instancers.find(lower_case_name);
  if (itr != element_instancers.end()) {
    (*itr).second->RemoveReference();
  }

  element_instancers[lower_case_name] = instancer;
  return instancer;
}

// Looks up the instancer for the given element
ElementInstancer* Factory::GetElementInstancer(
    Context* _context, const String& tag) {
  auto& element_instancers = (*_context)(kElementKey);
  ElementInstancerMap::iterator instancer_iterator =
      element_instancers.find(tag);
  if (instancer_iterator == element_instancers.end()) {
    instancer_iterator = element_instancers.find("*");
    if (instancer_iterator == element_instancers.end())
      return NULL;
  }

  return (*instancer_iterator).second;
}

// Instances a single element.
Element* Factory::InstanceElement(Context* _context, Element* parent,
    const String& instancer_name, const String& tag,
    const XMLAttributes& attributes) {
  ElementInstancer* instancer = GetElementInstancer(_context, instancer_name);

  if (instancer) {
    Element* element =
        instancer->InstanceElement(_context, parent, tag, attributes);

    // Process the generic attributes and bind any events
    if (element) {
      element->SetInstancer(instancer);
      element->SetAttributes(&attributes);
      ElementUtilities::BindEventAttributes(element);

      PluginRegistry::NotifyElementCreate(_context, element);
    }

    return element;
  }

  return NULL;
}

// Instances a single text element containing a string.
bool Factory::InstanceElementText(
    Context* _context, Element* parent, const String& text) {
  SystemInterface* system_interface = GetSystemInterface(_context);

  // Do any necessary translation. If any substitutions were made then new XML
  // may have been introduced, so we'll
  // have to run the data through the XML parser again.
  String translated_data;
  if (system_interface != NULL &&
      (system_interface->TranslateString(translated_data, text) > 0 ||
          translated_data.Find("<") != String::npos)) {
    StreamMemory* stream = new StreamMemory(translated_data.Length() + 32);
    stream->Write("<body>", 6);
    stream->Write(translated_data);
    stream->Write("</body>", 7);
    stream->Seek(0, SEEK_SET);

    InstanceElementStream(_context, parent, stream);
    stream->RemoveReference();
  } else {
    // Check if this text node contains only white-space; if so, we don't want
    // to construct it.
    bool only_white_space = true;
    for (size_t i = 0; i < translated_data.Length(); ++i) {
      if (!StringUtilities::IsWhitespace(translated_data[i])) {
        only_white_space = false;
        break;
      }
    }

    if (only_white_space)
      return true;

    // Attempt to instance the element.
    XMLAttributes attributes;
    Element* element = Factory::InstanceElement(
        _context, parent, "#text", "#text", attributes);
    if (!element) {
      Log::Message(_context, Log::LT_ERROR,
          "Failed to instance text element '%s', instancer returned NULL.",
          translated_data.CString());
      return false;
    }

    ElementText* text_element = rocket_dynamic_cast<ElementText*>(element);
    if (text_element == NULL) {
      Log::Message(_context, Log::LT_ERROR,
          "Failed to instance text element '%s'."
          "Expecting a derivative of ElementText.",
          translated_data.CString());
      element->RemoveReference();
      return false;
    }

    text_element->SetText(translated_data);

    // Add to active node.
    parent->AppendChild(element);
    element->RemoveReference();
  }

  return true;
}

// Instances a element tree based on the stream
bool Factory::InstanceElementStream(
    Context* _context, Element* parent, Stream* stream) {
  XMLParser parser(_context, parent);
  parser.Parse(stream);
  return true;
}

// Instances a element tree based on the stream
ElementDocument* Factory::InstanceDocumentStream(
    Context* _context, Rocket::Core::DocumentContext* context, Stream* stream) {
  Element* element =
      Factory::InstanceElement(_context, NULL, "body", "body", XMLAttributes());
  if (!element) {
    Log::Message(_context, Log::LT_ERROR,
        "Failed to instance document, instancer returned NULL.");
    return NULL;
  }

  ElementDocument* document = rocket_dynamic_cast<ElementDocument*>(element);
  if (!document) {
    Log::Message(_context, Log::LT_ERROR,
        "Failed to instance document element. Was expecting "
        "derivative of ElementDocument.");
    return NULL;
  }

  document->lock_layout = true;
  document->context = context;

  XMLParser parser(_context, element);
  parser.Parse(stream);

  document->lock_layout = false;

  return document;
}

// Registers an instancer that will be used to instance decorators.
DecoratorInstancer* Factory::RegisterDecoratorInstancer(
    Context* _context, const String& name, DecoratorInstancer* instancer) {
  auto& decorator_instancers = (*_context)(kDecoratorKey);
  String lower_case_name = name.ToLower();
  instancer->AddReference();

  // Check if an instancer for this tag is already defined. If so, release it.
  DecoratorInstancerMap::iterator iterator =
      decorator_instancers.find(lower_case_name);
  if (iterator != decorator_instancers.end())
    (*iterator).second->RemoveReference();

  decorator_instancers[lower_case_name] = instancer;
  return instancer;
}

// Attempts to instance a decorator from an instancer registered with the
// factory.
Decorator* Factory::InstanceDecorator(Context* _context, const String& name,
    const PropertyDictionary& properties) {
  auto& decorator_instancers = (*_context)(kDecoratorKey);
  float z_index = 0;
  int specificity = -1;

  DecoratorInstancerMap::iterator iterator = decorator_instancers.find(name);
  if (iterator == decorator_instancers.end())
    return NULL;

  // Turn the generic, un-parsed properties we've got into a properly parsed
  // dictionary.
  const PropertySpecification& property_specification =
      (*iterator).second->GetPropertySpecification();

  PropertyDictionary parsed_properties;
  for (PropertyMap::const_iterator i = properties.GetProperties().begin();
       i != properties.GetProperties().end(); ++i) {
    specificity = Math::Max(specificity, (*i).second.specificity);

    // Check for the 'z-index' property; we don't want to send this through.
    if ((*i).first == Z_INDEX)
      z_index = (*i).second.value.Get<float>();
    else
      property_specification.ParsePropertyDeclaration(parsed_properties,
          (*i).first, (*i).second.value.Get<String>(), (*i).second.source,
          (*i).second.source_line_number);
  }

  // Set the property defaults for all unset properties.
  property_specification.SetPropertyDefaults(parsed_properties);

  Decorator* decorator =
      (*iterator).second->InstanceDecorator(name, parsed_properties);
  if (decorator == NULL)
    return NULL;

  decorator->SetZIndex(z_index);
  decorator->SetSpecificity(specificity);
  decorator->instancer = (*iterator).second;
  return decorator;
}

// Registers an instancer that will be used to instance font effects.
FontEffectInstancer* Factory::RegisterFontEffectInstancer(
    Context* _context, const String& name, FontEffectInstancer* instancer) {
  auto& font_effect_instancers = (*_context)(kFontKey);
  String lower_case_name = name.ToLower();
  instancer->AddReference();

  // Check if an instancer for this tag is already defined. If so, release it.
  FontEffectInstancerMap::iterator iterator =
      font_effect_instancers.find(lower_case_name);
  if (iterator != font_effect_instancers.end())
    (*iterator).second->RemoveReference();

  font_effect_instancers[lower_case_name] = instancer;
  return instancer;
}

// Attempts to instance a font effect from an instancer registered with the
// factory.
FontEffect* Factory::InstanceFontEffect(Context* _context, const String& name,
    const PropertyDictionary& properties) {
  auto& font_effect_instancers = (*_context)(kFontKey);
  bool set_colour = false;
  Colourb colour(255, 255, 255);

  bool set_z_index = false;
  float z_index = 0;

  int specificity = -1;

  FontEffectInstancerMap::iterator iterator = font_effect_instancers.find(name);
  if (iterator == font_effect_instancers.end())
    return NULL;

  FontEffectInstancer* instancer = iterator->second;

  // Turn the generic, un-parsed properties we've got into a properly parsed
  // dictionary.
  const PropertySpecification& property_specification =
      (*iterator).second->GetPropertySpecification();

  PropertyDictionary parsed_properties;
  for (PropertyMap::const_iterator i = properties.GetProperties().begin();
       i != properties.GetProperties().end(); ++i) {
    specificity = Math::Max(specificity, i->second.specificity);

    // Check for the 'z-index' property; we don't want to send this through.
    if (i->first == Z_INDEX) {
      set_z_index = true;
      z_index = i->second.value.Get<float>();
    } else if (i->first == COLOR) {
      static PropertyParserColour colour_parser;

      Property colour_property;
      if (colour_parser.ParseValue(
              colour_property, i->second.value.Get<String>(), ParameterMap())) {
        colour = colour_property.value.Get<Colourb>();
        set_colour = true;
      }
    } else {
      property_specification.ParsePropertyDeclaration(parsed_properties,
          (*i).first, (*i).second.value.Get<String>(), (*i).second.source,
          (*i).second.source_line_number);
    }
  }

  // Set the property defaults for all unset properties.
  property_specification.SetPropertyDefaults(parsed_properties);

  // Compile an ordered list of the values of the properties used to generate
  // the effect's
  // textures and geometry.
  typedef std::list<std::pair<String, String>> GenerationPropertyList;
  GenerationPropertyList generation_properties;
  for (PropertyMap::const_iterator i =
           parsed_properties.GetProperties().begin();
       i != parsed_properties.GetProperties().end(); ++i) {
    if (instancer->volatile_properties.find(i->first) !=
        instancer->volatile_properties.end()) {
      GenerationPropertyList::iterator j = generation_properties.begin();
      while (j != generation_properties.end() && j->first < i->first)
        ++j;

      generation_properties.insert(
          j, GenerationPropertyList::value_type(
                 i->first, i->second.value.Get<String>()));
    }
  }

  String generation_key;
  for (GenerationPropertyList::iterator i = generation_properties.begin();
       i != generation_properties.end(); ++i) {
    generation_key += i->second;
    generation_key += ";";
  }

  // Now we can actually instance the effect!
  FontEffect* font_effect =
      (*iterator).second->InstanceFontEffect(_context, name, parsed_properties);
  if (font_effect == NULL)
    return NULL;

  font_effect->name = name;
  font_effect->generation_key = generation_key;

  if (set_z_index)
    font_effect->SetZIndex(z_index);

  if (set_colour)
    font_effect->SetColour(colour);

  font_effect->SetSpecificity(specificity);
  font_effect->instancer = (*iterator).second;
  return font_effect;
}

// Creates a style sheet containing the passed in styles.
StyleSheet* Factory::InstanceStyleSheetString(
    Context* _context, const String& string) {
  StreamMemory* memory_stream =
      new StreamMemory((const byte*)string.CString(), string.Length());
  StyleSheet* style_sheet = InstanceStyleSheetStream(_context, memory_stream);
  memory_stream->RemoveReference();
  return style_sheet;
}

// Creates a style sheet from a file.
StyleSheet* Factory::InstanceStyleSheetFile(
    Context* _context, const String& file_name) {
  StreamFile* file_stream = new StreamFile(_context);
  file_stream->Open(file_name);
  StyleSheet* style_sheet = InstanceStyleSheetStream(_context, file_stream);
  file_stream->RemoveReference();
  return style_sheet;
}

// Creates a style sheet from an Stream.
StyleSheet* Factory::InstanceStyleSheetStream(
    Context* _context, Stream* stream) {
  StyleSheet* style_sheet = new StyleSheet(_context);
  if (style_sheet->LoadStyleSheet(stream)) {
    return style_sheet;
  }

  style_sheet->RemoveReference();
  return NULL;
}

// Clears the style sheet cache. This will force style sheets to be reloaded.
void Factory::ClearStyleSheetCache(Context* _context) {
  StyleSheetFactory::ClearStyleSheetCache(_context);
}

/// Clears the template cache. This will force templates to be reloaded.
void Factory::ClearTemplateCache(Context* _context) {
  TemplateCache::Clear(_context);
}

// Registers an instancer for all RKTEvents
EventInstancer* Factory::RegisterEventInstancer(
    Context* _context, EventInstancer* instancer) {
  auto& event_instancer = (*_context)(kEventKey);
  instancer->AddReference();

  if (event_instancer)
    event_instancer->RemoveReference();

  event_instancer = instancer;
  return instancer;
}

// Instance an event object.
Event* Factory::InstanceEvent(Context* _context, Element* target,
    const String& name, const Dictionary& parameters, bool interruptible) {
  auto& event_instancer = (*_context)(kEventKey);
  Event* event =
      event_instancer->InstanceEvent(target, name, parameters, interruptible);
  if (event != NULL)
    event->instancer = event_instancer;

  return event;
}

// Register an instancer for all event listeners
EventListenerInstancer* Factory::RegisterEventListenerInstancer(
    Context* _context, EventListenerInstancer* instancer) {
  auto& event_listener_instancer = (*_context)(kListenerKey);
  instancer->AddReference();

  if (event_listener_instancer)
    event_listener_instancer->RemoveReference();

  event_listener_instancer = instancer;
  return instancer;
}

// Instance an event listener with the given string
EventListener* Factory::InstanceEventListener(
    Context* _context, const String& value, Element* element) {
  auto& event_listener_instancer = (*_context)(kListenerKey);
  // If we have an event listener instancer, use it
  if (event_listener_instancer)
    return event_listener_instancer->InstanceEventListener(value, element);

  return NULL;
}
}  // namespace Core
}  // namespace Rocket
