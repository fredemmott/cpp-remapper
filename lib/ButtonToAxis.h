/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"

#include <vector>
#include <cstdint>

namespace fredemmott::inputmapping::actions {

class ButtonToAxis final : public ButtonSink, public AxisSource {
 public:
  void map(bool value);
};

} // namespace fredemmott::inputmapping
