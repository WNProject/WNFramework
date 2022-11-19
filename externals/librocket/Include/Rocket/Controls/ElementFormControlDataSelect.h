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

#ifndef ROCKETCONTROLSELEMENTFORMCONTROLDATASELECT_H
#define ROCKETCONTROLSELEMENTFORMCONTROLDATASELECT_H

#include "DataSourceListener.h"
#include "ElementFormControlSelect.h"

namespace Rocket {
namespace Controls {

class DataSource;

/**
        A drop-down select form control driven from a data source.

        @author Peter Curry
 */

class ElementFormControlDataSelect : public ElementFormControlSelect,
                                     public DataSourceListener {
public:
  ROCKET_RTTI_DefineWithParent(ElementFormControlSelect);
  /// Constructs a new ElementFormControlDataSelect. This should not be called
  /// directly; use the
  /// Factory instead.
  /// @param[in] tag The tag the element was declared as in RML.
  ElementFormControlDataSelect(
      Core::Context* _context, const Rocket::Core::String& tag);
  virtual ~ElementFormControlDataSelect();

  /// Sets the data source the control's options are driven from.
  /// @param[in] data_source The name of the new data source.
  void SetDataSource(const Rocket::Core::String& data_source);

protected:
  /// If a new data source has been set on the control, this will attach to it
  /// and build the
  /// initial options.
  virtual void OnUpdate() override;

  /// Checks for changes to the data source or formatting attributes.
  /// @param[in] changed_attributes List of changed attributes on the element.
  virtual void OnAttributeChange(
      const Core::AttributeNameList& changed_attributes) override;

  /// Detaches from the data source and rebuilds the options.
  virtual void OnDataSourceDestroy(DataSource* data_source) override;
  /// Rebuilds the available options from the data source.
  virtual void OnRowAdd(DataSource* data_source,
      const Rocket::Core::String& table, int first_row_added,
      int num_rows_added) override;
  /// Rebuilds the available options from the data source.
  virtual void OnRowRemove(DataSource* data_source,
      const Rocket::Core::String& table, int first_row_removed,
      int num_rows_removed) override;
  /// Rebuilds the available options from the data source.
  virtual void OnRowChange(DataSource* data_source,
      const Rocket::Core::String& table, int first_row_changed,
      int num_rows_changed) override;
  /// Rebuilds the available options from the data source.
  virtual void OnRowChange(
      DataSource* data_source, const Rocket::Core::String& table) override;

private:
  // Builds the option list from the data source.
  void BuildOptions();

  DataSource* data_source;
  Rocket::Core::String data_table;

  bool initialised;
};
}  // namespace Controls
}  // namespace Rocket

#endif
