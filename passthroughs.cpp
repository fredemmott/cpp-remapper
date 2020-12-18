#include "passthroughs.h"

#include "vjoypp.h"

using fredemmott::vjoypp::OutputDevice;

namespace fredemmott::inputmapping {

VJoyAxis::VJoyAxis(
  const AxisTarget& target
): AxisAction(), mTarget(target) {
}

void VJoyAxis::map(long value) {
  (mTarget.device->*(mTarget.setter))(value);
}

VJoyButton::VJoyButton(
  const ButtonTarget& target
): ButtonAction(), mTarget(target) {
}

void VJoyButton::map(bool value) {
  mTarget.device->setButton(mTarget.button, value);
}

VJoyHat::VJoyHat(
  const HatTarget& target
): HatAction(), mTarget(target) {
}

void VJoyHat::map(int16_t value) {
  mTarget.device->setHat(mTarget.hat, value);
}

} // namespace fredemmott::inputmapping
