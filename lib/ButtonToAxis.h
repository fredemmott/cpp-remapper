/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "Sink.h"
#include "Source.h"

namespace fredemmott::inputmapping {

/** Convert a button to an axis.
 *
 * This is useful when binding a button to a virtual X360 trigger.
 */
class ButtonToAxis final : public ButtonSink, public AxisSource {
 private:
  Axis::Value mFalse;
  Axis::Value mTrue;

 public:
  ButtonToAxis(
    Axis::Value falseValue = Axis::MIN,
    Axis::Value trueValue = Axis::MAX);
  virtual void map(Button::Value value) override;
};

}// namespace fredemmott::inputmapping
