/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "axiscurve.h"

#include <cmath>

namespace fredemmott::inputmapping::actions {

AxisCurve::AxisCurve(
  float curviness
): mCurviness(curviness) {}

AxisCurve::~AxisCurve() {}

void AxisCurve::map(long value) {
  // Normalize between -1 to 1
  //
  // the raw range is 0-0xffff with 0x7fff as 'neutral', which means
  // there (0xffff - 0x7fff = 0x8000) possible positive values, but
  // (0x7fff - 0 = 0x7fff) negative values.
  //
  // This level of accuracy doesn't usually matter, but some games care
  // about actually reaching min and max values
  const auto scale = value > 0x7fff ? 0x8000 : 0x7fff;

  const double x = (value - 0x7fff) / (double) scale;
  const double k = mCurviness;
  // This is based on
  // https://dinodini.wordpress.com/2010/04/05/normalized-tunable-sigmoid-functions/
  const auto fx = (x - (x * k)) / (k - (abs(x) * 2 * k) + 1);

  const auto clamped = fx > 1.0 ? 1.0 : (fx < -1.0 ? -1.0 : fx);

  emit((clamped * scale) + 0x7fff);
}

} // namespace fredemmott::inputmapping::actions
