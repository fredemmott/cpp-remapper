/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>
#include <cstdint>

namespace fredemmott::inputmapping {

class Control {};

class Axis final : public Control {
 public:
  using Value = long;
  static const Value MAX = 0xffff;
  static const Value MIN = 0;
  // Not called 'center' because that's misleading for sliders.
  static const Value MID = MAX / 2;
};

class Button final : public Control {
 public:
  using Value = bool;
};

class Hat final : public Control {
 public:
  using Value = uint16_t;
  static const Value MAX = 35999;
  static const Value MIN = 0;
  static const Value CENTER = 0xffff;

  static const Value NORTH = 0;
  static const Value NORTH_EAST = 4500;
  static const Value EAST = 9000;
  static const Value SOUTH_EAST = 13500;
  static const Value SOUTH = 18000;
  static const Value SOUTH_WEST = 22500;
  static const Value WEST = 27000;
  static const Value NORTH_WEST = 31500;
};

}// namespace fredemmott::inputmapping
