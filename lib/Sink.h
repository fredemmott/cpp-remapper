/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <type_traits>

#include "Controls.h"

namespace fredemmott::inputmapping {

class AnySink {};

template <typename TControl>
class Sink : public AnySink {
  static_assert(std::is_base_of_v<Control, TControl>);

 protected:
  Sink() = default;

 public:
  using InControl = TControl;
  using In = typename TControl::Value;
  virtual void map(In value) = 0;
};
using AxisSink = Sink<Axis>;
using ButtonSink = Sink<Button>;
using HatSink = Sink<Hat>;

}// namespace fredemmott::inputmapping
