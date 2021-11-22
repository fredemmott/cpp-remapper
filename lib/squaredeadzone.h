/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cstdint>

#include "actionsapi.h"

namespace fredemmott::inputmapping {

/** A simple deadzone for a single axis.
 *
 * Values less than x% are ignored. Larger values are scaled to still report as
 * 0-100%; this leads to a loss of precision.
 *
 * For example, if you set a 50% dead zone, 50% deflection will be reported as
 * 0%, 75% as 50%, and 100% as 100%. This means that once you reach 50%, each
 * 1% change is reported as 2%, so the precision is half.
 *
 * As the original precision is reported out of 32767, this is unlikely to be a
 * practical concern, but you probably don't want to chain a bunch of deadzones
 * as this effect compounds.
 *
 * While this deadzone action only operates on a single axis at a time, if you
 * apply to both the X and Y axis, you end up with a square dead zone :)
 */
class SquareDeadzone : public AxisSink, public AxisSource {
 public:
  SquareDeadzone(uint8_t percent);
  virtual ~SquareDeadzone();
  virtual void map(long value) override;

 private:
  uint8_t mPercent;
};

}// namespace fredemmott::inputmapping
