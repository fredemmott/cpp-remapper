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
