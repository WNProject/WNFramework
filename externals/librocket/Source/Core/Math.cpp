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

#include "../../Include/Rocket/Core/Math.h"
#include <math.h>
#include <time.h>
#include "precompiled.h"

namespace Rocket {
namespace Core {
namespace Math {

const float ROCKET_PI = 3.141592653f;

static const float FZERO = 0.0001f;

// Evaluates if a number is, or close to, zero.
bool IsZero(float value) {
  return AbsoluteValue(value) < FZERO;
}

// Evaluates if two floating-point numbers are equal, or so similar that they
// could be considered
// so.
bool AreEqual(float value_0, float value_1) {
  return IsZero(value_1 - value_0);
}

// Calculates the absolute value of a number.
float AbsoluteValue(float value) {
  return fabsf(value);
}

// Calculates the cosine of an angle.
float Cos(float angle) {
  return cosf(angle);
}

// Calculates the arc-cosine of an value.
float ACos(float value) {
  return acos(value);
}

// Calculates the sine of an angle.
float Sin(float angle) {
  return sin(angle);
}

// Calculates the arc-sine of an value.
float ASin(float angle) {
  return asinf(angle);
}

// Calculates the tangent of an angle.
float Tan(float angle) {
  return tanf(angle);
}

// Calculates the angle of a two-dimensional line.
float ATan2(float y, float x) {
  return atan2f(y, x);
}

// Converts an angle from radians to degrees.
float RadiansToDegrees(float angle) {
  return angle * (180.0f / ROCKET_PI);
}

// Converts an angle from degrees to radians.
float DegreesToRadians(float angle) {
  return angle * (ROCKET_PI / 180.0f);
}

// Normalises and angle in radians
float NormaliseAngle(float angle) {
  return fmodf(angle, ROCKET_PI * 2.0f);
}

// Calculates the square root of a value.
float SquareRoot(float value) {
  return sqrtf(value);
}

// Rounds a floating-point value to the nearest integer.
int Round(float value) {
  if (value > 0.0f)
    return RealToInteger(value + 0.5f);

  return RealToInteger(value - 0.5f);
}

// Rounds a floating-point value up to the nearest integer.
int RoundUp(float value) {
  return RealToInteger(ceilf(value));
}

// Rounds a floating-point value down to the nearest integer.
int RoundDown(float value) {
  return RealToInteger(floorf(value));
}

// Efficiently truncates a floating-point value into an integer.
int RealToInteger(float value) {
  return (int)value;
}

// Converts the given number to a power of two, rounding up if necessary.
int ToPowerOfTwo(int number) {
  // Check if the number is already a power of two.
  if ((number & (number - 1)) == 0)
    return number;

  // Assuming 31 useful bits in an int here ... !
  for (int i = 31; i >= 0; i--) {
    if (number & (1 << i)) {
      if (i == 31)
        return 1 << 31;
      else
        return 1 << (i + 1);
    }
  }

  return 0;
}

// Converts from a hexadecimal digit to decimal.
int HexToDecimal(char hex_digit) {
  if (hex_digit >= '0' && hex_digit <= '9')
    return hex_digit - '0';
  else if (hex_digit >= 'a' && hex_digit <= 'f')
    return 10 + (hex_digit - 'a');
  else if (hex_digit >= 'A' && hex_digit <= 'F')
    return 10 + (hex_digit - 'A');

  return -1;
}

// Generates a random floating-point value between 0 and a user-specified value.
float RandomReal(float max_value) {
  return (rand() / (float)RAND_MAX) * max_value;
}

// Generates a random integer value between 0 and a user-specified value.
int RandomInteger(int max_value) {
  return (rand() % max_value);
}

// Generates a random boolean value, with equal chance of true or false.
bool RandomBool() {
  return RandomInteger(2) == 1;
}
}  // namespace Math
}  // namespace Core
}  // namespace Rocket
