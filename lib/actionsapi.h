/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <functional>
#include <memory>
#include <set>

/* This file contains definitions you're likely to need to implement
 * additional actions, if the provided ones are not sufficient.
 */

namespace fredemmott::gameinput {
  class InputDevice;
}

namespace fredemmott::inputmapping {

/** An action represents what happens in response to an input.
 *
 * This usually consists of code (in the map function) and data, such as
 * target button/axis information.
 */
template<typename TValue>
class Action {
 public:
   typedef TValue Value;
   virtual ~Action() {}
   virtual void map(TValue value) = 0;
};

/// An Action that happens in response to an axis movement
class AxisAction: public Action<long> {};
/// An Action that happens in response to a button movement
class ButtonAction : public Action<bool> {};
/// An action that happens in response to a POV hat movement
class HatAction : public Action<uint16_t> {};

typedef std::shared_ptr<AxisAction> SharedAxisAction;
typedef std::shared_ptr<ButtonAction> SharedButtonAction;
typedef std::shared_ptr<HatAction> SharedHatAction;

struct ButtonSource {
  fredemmott::gameinput::InputDevice* device;
  uint8_t button;
};

struct HatSource {
  fredemmott::gameinput::InputDevice* device;
  uint8_t hat;
};

struct AxisSource {
  fredemmott::gameinput::InputDevice* device;
  uint8_t axis;
};

} // namespace fredemmott::inputmapping
