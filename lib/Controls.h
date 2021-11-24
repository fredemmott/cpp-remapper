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
};

class Button final : public Control {
 public:
  using Value = bool;
};

class Hat final : public Control {
 public:
  using Value = uint16_t;
  static const Value MAX = 36000;
  static const Value MIN = 0;
  static const Value CENTER = 0xffff;
};

}// namespace fredemmott::inputmapping
