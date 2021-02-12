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
#ifndef ROCKETCORECORE_H
#define ROCKETCORECORE_H

#include "Box.h"
#include "Context.h"
#include "Decorator.h"
#include "DecoratorInstancer.h"
#include "DocumentContext.h"
#include "DocumentContextInstancer.h"
#include "Element.h"
#include "ElementDocument.h"
#include "ElementInstancer.h"
#include "ElementInstancerGeneric.h"
#include "ElementReference.h"
#include "ElementScroll.h"
#include "ElementText.h"
#include "ElementUtilities.h"
#include "Event.h"
#include "EventInstancer.h"
#include "EventListener.h"
#include "EventListenerInstancer.h"
#include "Factory.h"
#include "FileInterface.h"
#include "Font.h"
#include "FontDatabase.h"
#include "FontEffect.h"
#include "FontGlyph.h"
#include "Geometry.h"
#include "GeometryUtilities.h"

#include "Input.h"
#include "Log.h"
#include "Math.h"
#include "Plugin.h"
#include "Property.h"
#include "PropertyDefinition.h"
#include "PropertyDictionary.h"
#include "PropertyParser.h"
#include "PropertySpecification.h"
#include "RenderInterface.h"
#include "String.h"
#include "StyleSheet.h"
#include "StyleSheetKeywords.h"
#include "StyleSheetSpecification.h"
#include "SystemInterface.h"
#include "Texture.h"
#include "Types.h"
#include "Vertex.h"
#include "XMLNodeHandler.h"
#include "XMLParser.h"

namespace Rocket {
namespace Core {

class Context;
class Plugin;

/**
        Rocket library core API.

        @author Peter Curry
 */

/// Initialises Rocket.
bool Initialise(Context* _context);
/// Shutdown Rocket.
void Shutdown(Context* _context);

/// Returns the version of this Rocket library.
/// @return The version number.
String GetVersion();

/// Sets the interface through which all system requests are made. This must be
/// called before Initialise().
/// @param[in] system_interface The application-specified logging interface.
void SetSystemInterface(Context* _context, SystemInterface* system_interface);
/// Returns Rocket's system interface.
/// @return Rocket's system interface.
SystemInterface* GetSystemInterface(Context* _context);

/// Sets the interface through which all rendering requests are made. This is
/// not required to be called, but if it is
/// it must be called before Initialise(). If no render interface is specified,
/// then all contexts must have a custom
/// render interface.
/// @param[in] render_interface Render interface implementation.
void SetRenderInterface(Context* _context, RenderInterface* render_interface);
/// Returns Rocket's default's render interface.
/// @return Rocket's render interface.
RenderInterface* GetRenderInterface(Context* _context);

/// Sets the interface through which all file I/O requests are made. This is not
/// required to be called, but if it is it
/// must be called before Initialise().
/// @param[in] file_interface The application-specified file interface
void SetFileInterface(Context* _context, FileInterface* file_interface);
/// Returns Rocket's file interface.
/// @return Rocket's file interface.
FileInterface* GetFileInterface(Context* _context);

/// Creates a new element context.
/// @param[in] name The new name of the context. This must be unique.
/// @param[in] dimensions The initial dimensions of the new context.
/// @param[in] render_interface The custom render interface to use, or NULL to
/// use the default.
/// @return The new context, or NULL if the context could not be created.
DocumentContext* CreateDocumentContext(Context* _context, const String& name,
    const Vector2i& dimensions, RenderInterface* render_interface = NULL);
/// Fetches a previously constructed context by name.
/// @param[in] name The name of the desired context.
/// @return The desired context, or NULL if no context exists with the given
/// name.
DocumentContext* GetContext(Context* _context, const String& name);
/// Fetches a context by index.
/// @param[in] index The index of the desired context. If this is outside of the
/// valid range of contexts, it will be clamped.
/// @return The requested context, or NULL if no contexts exist.
DocumentContext* GetContext(Context* _context, int index);
/// Returns the number of active contexts.
/// @return The total number of active Rocket contexts.
int GetNumContexts(Context* _context);

/// Registers a generic Rocket plugin.
void RegisterPlugin(Context* _context, Plugin* plugin);

/// Forces all compiled geometry handles generated by libRocket to be released.
void ReleaseCompiledGeometries(Context* _context);
/// Forces all texture handles loaded and generated by libRocket to be released.
void ReleaseTextures(Context* _context);

void RegisterContextValues(Context* _context);
}  // namespace Core
}  // namespace Rocket

#endif
