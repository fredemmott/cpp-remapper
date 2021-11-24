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

class ButtonToAxis final : public ButtonSink, public AxisSource {
 public:
  void map(Button::Value value);
};

}// namespace fredemmott::inputmapping
