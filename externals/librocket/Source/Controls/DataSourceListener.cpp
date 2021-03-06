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
#include "../../Include/Rocket/Controls/DataSourceListener.h"
#include "../../Include/Rocket/Controls/DataSource.h"
#include "../../Include/Rocket/Core/Log.h"
#include "../../Include/Rocket/Core/StringUtilities.h"

namespace Rocket {
namespace Controls {

DataSourceListener::DataSourceListener(Core::Context* _context)
  : m_context(_context) {}

DataSourceListener::~DataSourceListener() {}

// Notification of the destruction of an observed data source.
void DataSourceListener::OnDataSourceDestroy(
    DataSource* ROCKET_UNUSED_PARAMETER(data_source)) {}

// Notification of the addition of one or more rows to an observed data source's
// table.
void DataSourceListener::OnRowAdd(
    DataSource* ROCKET_UNUSED_PARAMETER(data_source),
    const Rocket::Core::String& ROCKET_UNUSED_PARAMETER(table),
    int ROCKET_UNUSED_PARAMETER(first_row_added),
    int ROCKET_UNUSED_PARAMETER(num_rows_added)) {}

// Notification of the removal of one or more rows from an observed data
// source's table.
void DataSourceListener::OnRowRemove(
    DataSource* ROCKET_UNUSED_PARAMETER(data_source),
    const Rocket::Core::String& ROCKET_UNUSED_PARAMETER(table),
    int ROCKET_UNUSED_PARAMETER(first_row_removed),
    int ROCKET_UNUSED_PARAMETER(num_rows_removed)) {}

// Notification of the changing of one or more rows from an observed data
// source's table.
void DataSourceListener::OnRowChange(
    DataSource* ROCKET_UNUSED_PARAMETER(data_source),
    const Rocket::Core::String& ROCKET_UNUSED_PARAMETER(table),
    int ROCKET_UNUSED_PARAMETER(first_row_changed),
    int ROCKET_UNUSED_PARAMETER(num_rows_changed)) {}

// Notification of the change of all of the data of an observed data source's
// table.
void DataSourceListener::OnRowChange(
    DataSource* ROCKET_UNUSED_PARAMETER(data_source),
    const Rocket::Core::String& ROCKET_UNUSED_PARAMETER(table)) {}

// Sets up data source and table from a given string.
bool DataSourceListener::ParseDataSource(DataSource*& data_source,
    Rocket::Core::String& table_name,
    const Rocket::Core::String& data_source_name) {
  if (data_source_name.Length() == 0) {
    data_source = NULL;
    table_name = "";
    return false;
  }

  Rocket::Core::StringList data_source_parts;
  Rocket::Core::StringUtilities::ExpandString(
      data_source_parts, data_source_name, '.');

  DataSource* new_data_source =
      DataSource::GetDataSource(m_context, data_source_parts[0].CString());

  if (data_source_parts.size() != 2 || !new_data_source) {
    Rocket::Core::Log::Message(m_context, Rocket::Core::Log::LT_ERROR,
        "Bad data source name %s", data_source_name.CString());
    data_source = NULL;
    table_name = "";
    return false;
  }

  data_source = new_data_source;
  table_name = data_source_parts[1];
  return true;
}
}  // namespace Controls
}  // namespace Rocket
