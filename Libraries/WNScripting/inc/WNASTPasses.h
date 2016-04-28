// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_AST_PASSES_H__
#define __WN_SCRIPTING_AST_PASSES_H__
#include "WNCore/inc/WNTypes.h"

namespace WNLogging {
class WNLog;
}

namespace wn {
namespace scripting {
class script_file;
class type_validator;

// Runs the type_association pass. This is required to associate
// a type with all nodes who'se type is inferred from the context.
// For example the type of a binary expression depends on the type
// of its operands.
// Returns false, and writes to the _log parameter if a type
// could not be determined.
bool run_type_association_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors);

// Associates all IDs with the parameters/declaration of the id.
// Returns false, and writes to the _log parameter if an id was not
// accessible in the current scope.
bool run_id_association_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors);

// Removes dead code from the tree. Examples include:
// Code after return.
// TODO(awoloszyn): Remove constant branches.
bool run_dce_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors);

// Re-writes member functions and constructors as standalone
// functions.
// TODO(awoloszyn): also generate inline types/constructors here.
bool run_member_reassociation_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors);

// If reassociation pass
// Optional pass for Generators only supporting structure control.
// (Shading languages, C without Goto)
// This takes if statements of the form
//
// if (blah()) {
//   do thing;
// } else if (blar()) {
//   do second thing;
// } else {
//   do else thing;
// }
//
// and convertes them into the form.
//
// Bool _tmp = False;
// {
//   _tmp = blah();
// }
// if (_tmp) {
//   do thing;
// }
// {
//   _tmp = !_tmp && blar();
// }
// if (_tmp) {
//   do second thing;
// }
// if (!_tmp) {
//  do else thing
// }
// This should be run before the id_reassociation pass since this
// can generate new IDs on the fly.
bool run_if_reassociation_pass(script_file* _file, WNLogging::WNLog* _log,
    type_validator* _validator, size_t* _num_warnings, size_t* _num_errors);

// TODO(awoloszyn): Constant folding pass.

}  // namespace scripting
}  // namesapce wn
#endif  //__WN_SCRIPTING_AST_PASSES_H__
