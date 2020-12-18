#pragma once

#include <set>

/* This file contains definitions you're likely to need to implement
 * additional actions, if the provided ones are not sufficient.
 */

namespace fredemmott::gameinput {
  class InputDevice;
}

namespace fredemmott::vjoypp {
  class OutputDevice;
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
   typedef TValue value_type;
   virtual ~Action() {}
   virtual std::set<fredemmott::vjoypp::OutputDevice*> getAffectedDevices() = 0;
   virtual void map(TValue value) = 0;
};

/// An Action that happens in response to an axis movement
class AxisAction: public Action<long> {};
/// An Action that happens in response to a button movement
class ButtonAction : public Action<bool> {};
/// An action that happens in response to a POV hat movement
class HatAction : public Action<int16_t> {};

template<typename TDevice>
struct MappableButton {
  TDevice* device;
  uint8_t button;
};
typedef MappableButton<fredemmott::gameinput::InputDevice> ButtonSource;
typedef MappableButton<fredemmott::vjoypp::OutputDevice> ButtonTarget;

template<typename TDevice>
struct MappableHat {
  TDevice* device;
  uint8_t hat;
};
typedef MappableHat<fredemmott::gameinput::InputDevice> HatSource;
typedef MappableHat<fredemmott::vjoypp::OutputDevice> HatTarget;

struct AxisSource {
  fredemmott::gameinput::InputDevice* device;
  uint8_t axis;
};

struct AxisTarget {
  fredemmott::vjoypp::OutputDevice* device;
  const char* label;
  fredemmott::vjoypp::OutputDevice* (fredemmott::vjoypp::OutputDevice::*setter)(long);
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
