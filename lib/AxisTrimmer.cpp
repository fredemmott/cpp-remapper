/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <cpp-remapper/AxisTrimmer.h>

#include <algorithm>

namespace fredemmott::inputmapping {

void AxisTrimmer::map(Axis::Value v) {
  mValue = v;
  update();
}

void AxisTrimmer::update() {
  if (mTrimming) {
    return;
  }
  emit(std::clamp(mValue + mOffset, Axis::MIN, Axis::MAX));
}

void AxisTrimmer::trim(bool pressed) {
  if (pressed) {
    mTrimming = true;
    mStartValue = mValue;
    return;
  }

  if (!mTrimming) {
    return;
  }

  mTrimming = false;

  mOffset += mStartValue - mValue;

  constexpr auto max_offset = Axis::MID + 1;
  mOffset = std::clamp(mOffset, -max_offset, max_offset);

  update();
}

void AxisTrimmer::reset() {
  mOffset = 0;
  update();
}

}// namespace fredemmott::inputmapping
