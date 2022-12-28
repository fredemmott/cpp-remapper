/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cpp-remapper/Sink.h>
#include <cpp-remapper/Source.h>

namespace fredemmott::inputmapping {

/** A simple curve with tuneable 'curviness'.
 *
 * - Curviness is >= -1 and < 1
 * - Curviness of 0 no curve, i.e. a straight line
 * - Curviness of 0.9999999 is a very slightly rounded right angle corner
 * - You probably want curvature between 0 and 1; 0.5 is a good starting point.
 *   This makes the axis less sensitive near center, more sensitive when fully
 *   deflective. A negative curviness gives you the opposite.
 */
class AxisCurve final : public AxisSource, public AxisSink {
 public:
  AxisCurve(double curviness);
  virtual ~AxisCurve();
  void map(long value);

 private:
  double mCurviness;
};

}// namespace fredemmott::inputmapping
