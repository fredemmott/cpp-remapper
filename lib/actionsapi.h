/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <functional>
#include <set>

/* This file contains definitions you're likely to need to implement
 * additional actions, if the provided ones are not sufficient.
 */

namespace fredemmott::gameinput {
  class InputDevice;
}

namespace fredemmott::inputmapping {
  class VJoyOutput;
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

template<typename TDevice>
struct MappableButton {
  TDevice* device;
  uint8_t button;
};
typedef MappableButton<fredemmott::gameinput::InputDevice> ButtonSource;
struct ButtonTarget : public MappableButton<fredemmott::inputmapping::VJoyOutput> {
  void set(bool) const;
};

template<typename TDevice>
struct MappableHat {
  TDevice* device;
  uint8_t hat;
};
typedef MappableHat<fredemmott::gameinput::InputDevice> HatSource;
struct HatTarget : public MappableHat<fredemmott::inputmapping::VJoyOutput> {
  void set(uint16_t value) const;
};

struct AxisSource {
  fredemmott::gameinput::InputDevice* device;
  uint8_t axis;
};

struct AxisTarget {
  fredemmott::inputmapping::VJoyOutput* device;
  const char* label;
  fredemmott::inputmapping::VJoyOutput* (fredemmott::inputmapping::VJoyOutput::*setter)(long);

  void set(long value) const;
};
/* This was a few hours of "why is a constructor that just initializes constants
 * overwritting something else on the stack?!"
 */
static_assert(
  sizeof(AxisTarget::setter) > sizeof(void*),
  "Compile with /vmg for correct behavior of member function pointers with "
  "forward-declared classes. Otherwise, sizeof() varies depending on if you"
  "have the full declaration or not."
);

} // namespace fredemmott::inputmapping
