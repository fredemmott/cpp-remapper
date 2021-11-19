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

#define AXIS_TYPES AT(X) AT(Y) AT(Z) AT(RX) AT(RY) AT(RZ) AT(SLIDER)
enum class AxisType {
#define AT(x) x,
  AXIS_TYPES
#undef AT
};

struct AxisInformation {
  AxisType type;
  std::string name = "Invalid axis type";

  AxisInformation() = delete;
  AxisInformation(AxisType type, std::string name) : type(type), name(name) {
  }

  AxisInformation(AxisType type) : type(type) {
    switch (type) {
#define AT(x) \
  case AxisType::x: \
    name = #x; \
    break;
      AXIS_TYPES
#undef AT
    }
  }
};
#undef AXIS_TYPES

}// namespace fredemmott::gameinput
