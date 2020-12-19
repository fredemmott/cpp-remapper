/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <string>

namespace fredemmott::gameinput {

enum class AxisType {
  X,
  Y,
  Z,
  RX,
  RY,
  RZ,
  SLIDER,
};

struct AxisInformation {
  AxisType type;
  std::string name;
};

} // namespace fredemmott::gameinput
