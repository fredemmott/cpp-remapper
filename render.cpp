/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

// This file is an executable, intended to generate illustrative diagrams
// of some basic axis mapping operations

#include "lib/axiscurve.h"
#include "lib/easymode.h"
#include "lib/render_axis.h"

using fredemmott::inputmapping::render_axis;
using fredemmott::inputmapping::Sink;
using fredemmott::inputmapping::Source;
using fredemmott::inputmapping::maybe_shared_ptr;

int main() {
  render_axis("linear.bmp", [](Axis::Value next) { return next; });
  render_axis("curve_0.bmp", AxisCurve(0));
  render_axis("curve_0.5.bmp", AxisCurve(0.5));
  render_axis("curve_neg0.5.bmp", AxisCurve(-0.5));
  render_axis("curve_0.99.bmp", AxisCurve(0.99));
  render_axis("dead_10.bmp", SquareDeadzone(10));

  auto x = std::make_shared<SquareDeadzone>(50) >> std::make_shared<AxisCurve>(0.5);
  auto y = std::make_shared<SquareDeadzone>(50) >> AxisCurve(0.5);
  auto z = SquareDeadzone(50) >> AxisCurve(0.5);

  render_axis("dead_curve.bmp", SquareDeadzone(50) >> AxisCurve(0.5));
  render_axis("curve_dead.bmp", AxisCurve(0.5) >> SquareDeadzone(50));
  return 0;
}
