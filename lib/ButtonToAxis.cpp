/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <cpp-remapper/ButtonToAxis.h>

namespace fredemmott::inputmapping {

ButtonToAxis::ButtonToAxis(Axis::Value falseValue, Axis::Value trueValue)
  : mFalse(falseValue), mTrue(trueValue) {
}

void ButtonToAxis::map(bool value) {
  emit(value ? mTrue : mFalse);
}

}// namespace fredemmott::inputmapping
