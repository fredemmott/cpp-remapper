/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"
#include "eventhandler.h"

#include <vector>
#include <cstdint>

namespace fredemmott::inputmapping::actions {

class ButtonToAxis : public ButtonAction {
 public:
  ButtonToAxis(AxisEventHandler next);
  void map(bool value);
 private:
  AxisEventHandler mNext;
};

} // namespace fredemmott::inputmapping
