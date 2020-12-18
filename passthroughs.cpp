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

std::set<OutputDevice*> VJoyAxis::getAffectedDevices() {
  return { mTarget.device };
}

VJoyButton::VJoyButton(
  const ButtonTarget& target
): ButtonAction(), mTarget(target) {
}

void VJoyButton::map(bool value) {
  mTarget.device->setButton(mTarget.button, value);
}

std::set<OutputDevice*> VJoyButton::getAffectedDevices() {
  return { mTarget.device };
}

VJoyHat::VJoyHat(
  const HatTarget& target
): HatAction(), mTarget(target) {
}

void VJoyHat::map(int16_t value) {
  mTarget.device->setHat(mTarget.hat, value);
}

std::set<OutputDevice*> VJoyHat::getAffectedDevices() {
  return { mTarget.device };
}

} // namespace fredemmott::inputmapping
