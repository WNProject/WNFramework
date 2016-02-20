// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_ENUMS_H__
#define __WN_SCRIPTING_ENUMS_H__
#include "WNCore/inc/WNTypes.h"

enum WNAssignType {
  AT_EQ,
  AT_ADD_EQ,
  AT_SUB_EQ,
  AT_MULT_EQ,
  AT_DIV_EQ,
  AT_MOD_EQ,
  AT_CHOWN,
  AT_MAX
};

static const char* WNAssignTypeNames[AT_MAX] = {
    "=", "+=", "-=", "*=", "/=", "%=", "<=="};

enum WNSSType { ST_AND, ST_OR, ST_MAX };

static const char* WNSSTypeNames[ST_MAX] = {"And", "Or"};

enum WNEQType { EQ_EQ, EQ_NEQ, EQ_LT, EQ_GT, EQ_LEQ, EQ_GEQ, EQ_MAX };

// The only place these are used is to define constants that are handled in a
// special way
// special insofar as the language recognizes them
enum WNScriptTypeName {
  SC_INT,
  SC_FLOAT,
  SC_CHAR,
  SC_STRING,
  SC_BOOL,
  SC_CUSTOM,
  SC_MAX
};

static const char* WNScriptTypeNames[SC_MAX] = {
    "Int", "Float", "Char", "Char", "Bool", "Custom"};

static const size_t WNScriptTypeLevels[SC_MAX] = {0, 0, 0, 1, 0, 0};

enum WNArithmeticType {
  AR_ADD,
  AR_SUB,
  AR_MULT,
  AR_DIV,
  AR_MOD,
  AR_EQ,
  AR_NEQ,
  AR_LEQ,
  AR_GEQ,
  AR_LT,
  AR_GT,
  AR_MAX
};

static const char* WNArithmeticTypeNames[AR_MAX] = {
    "+", "-", "*", "/", "%", "==", "!=", "<=", ">=", "<", ">"};

enum WNUnaryType { UN_PREINC, UN_PREDEC, UN_NEG, UN_MAX };

static const char* WNUnaryTypeNames[UN_MAX] = {"++", "--", "-"};

enum WNPostUNType { UN_POSTINC, UN_POSTDEC, UN_POSTMAX };

static const char* WNPostUNTypeNames[UN_POSTMAX] = {"++", "--"};

#endif  //__WN_SCRIPTING_ENUMS_H__
