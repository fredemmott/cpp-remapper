/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

// This file is an executable, intended to generate illustrative diagrams
// of some basic axis mapping operations

#include "lib/easymode.h"
#include "lib/render_axis.h"

using fredemmott::inputmapping::render_axis;
using fredemmott::inputmapping::AxisEventHandler;

int main() {
  render_axis("linear.bmp", [](auto next) { return next; });
  render_axis("curve_0.bmp", [](auto next) { return AxixCurve { 0, next }; });
  render_axis("curve_0.5.bmp", [](auto next) { return AxixCurve { 0.5, next }; });
  render_axis("curve_neg0.5.bmp", [](auto next) { return AxixCurve { -0.5, next }; });
  render_axis("curve_0.99.bmp", [](auto next) { return AxixCurve { 0.99, next }; });
  render_axis("dead_10.bmp", [](auto next) { return SquareDeadzone { 10, next }; });
  render_axis("dead_curve.bmp", [](auto next) { return SquareDeadzone { 50, AxisCurve { 0.5, next } }; });
  render_axis("curve_dead.bmp", [](auto next) { return AxisCurve { 0.5, SquareDeadzone { 50, next } }; });
  return 0;
}
