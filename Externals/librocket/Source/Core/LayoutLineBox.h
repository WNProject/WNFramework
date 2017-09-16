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
#ifndef ROCKETCORELAYOUTLINEBOX_H
#define ROCKETCORELAYOUTLINEBOX_H

#include "LayoutInlineBox.h"

namespace Rocket {
namespace Core {
class Context;
class LayoutBlockBox;

/**
        @author Peter Curry
 */

class LayoutLineBox {
public:
  LayoutLineBox(Context* _context, LayoutBlockBox* parent);
  ~LayoutLineBox();

  /// Closes the line box, positioning all inline elements within it.
  /// @param overflow[in] The overflow box from a split inline box that caused
  /// this line to close. Leave this as NULL if we closed naturally.
  /// @return If there was any overflow, this will be the last element generated
  /// by the spilling content. Otherwise, this will be NULL.
  LayoutInlineBox* Close(LayoutInlineBox* overflow = NULL);

  /// Closes one of the line box's inline boxes.
  /// @param inline_box[in] The inline box to close. This should always be the
  /// line box's open box.
  void CloseInlineBox(LayoutInlineBox* inline_box);

  /// Attempts to add a new element to this line box. If it can't fit, or needs
  /// to be split, new line boxes will
  /// be created. The inline box for the final section of the element will be
  /// returned.
  /// @param element[in] The element to fit into this line box.
  /// @param box[in] The element's extents.
  /// @return The inline box for the element.
  LayoutInlineBox* AddElement(Element* element, const Box& box);

  /// Attempts to add a new inline box to this line. If it can't fit, or needs
  /// to be split, new line boxes will
  /// be created. The inline box for the final section of the element will be
  /// returned.
  /// @param box[in] The inline box to be added to the line.
  /// @return The final inline box.
  LayoutInlineBox* AddBox(LayoutInlineBox* box);
  /// Adds an inline box as a chained hierarchy overflowing to this line. The
  /// chain will be extended into
  /// this line box.
  /// @param split_box[in] The box overflowed from a previous line.
  void AddChainedBox(LayoutInlineBox* chained_box);

  /// Returns the position of the line box, relative to its parent's block box's
  /// content area.
  /// @return The position of the line box.
  const Vector2f& GetPosition() const;
  /// Returns the position of the line box, relative to its parent's block box's
  /// offset parent.
  /// @return The relative position of the line box.
  Vector2f GetRelativePosition() const;
  /// Returns the dimensions of the line box.
  /// @return The dimensions of the line box.
  const Vector2f& GetDimensions() const;

  /// Returns the line box's open inline box.
  /// @return The line's open inline box, or NULL if it currently has none.
  LayoutInlineBox* GetOpenInlineBox();
  /// Returns the line's containing block box.
  /// @return The line's block box.
  LayoutBlockBox* GetBlockBox();

private:
  /// Appends an inline box to the end of the line box's list of inline boxes.
  void AppendBox(LayoutInlineBox* box);

  typedef std::vector<LayoutInlineBox*> InlineBoxList;

  // The block box containing this line.
  LayoutBlockBox* parent;

  // The top-left position of the line box; this is set when the first inline
  // box is placed.
  Vector2f position;
  bool position_set;

  // The width and height of the line box; this is set when the line box is
  // placed.
  Vector2f dimensions;
  bool wrap_content;

  // The horizontal cursor. This is where the next inline box will be placed
  // along the line.
  float box_cursor;

  // The list of inline boxes in this line box. These line boxes may be parented
  // to others in this list.
  InlineBoxList inline_boxes;
  // The open inline box; this is NULL if all inline boxes are closed.
  LayoutInlineBox* open_inline_box;

  Context* m_context;
};
}  // namespace Core
}  // namespace Rocket

#endif
