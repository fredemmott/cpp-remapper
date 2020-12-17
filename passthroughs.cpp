#include "passthroughs.h"

#include "vjoypp.h"

using fredemmott::vjoypp::OutputDevice;

namespace fredemmott::inputmapping {

AxisPassthrough::AxisPassthrough(
  const AxisTarget& target
): AxisAction(), mTarget(target) {
}

void AxisPassthrough::map(long value) {
  (mTarget.device->*(mTarget.setter))(value);
}

std::set<OutputDevice*> AxisPassthrough::getAffectedDevices() {
  return { mTarget.device };
}

ButtonPassthrough::ButtonPassthrough(
  const ButtonTarget& target
): ButtonAction(), mTarget(target) {
}

void ButtonPassthrough::map(bool value) {
  mTarget.device->setButton(mTarget.button, value);
}

std::set<OutputDevice*> ButtonPassthrough::getAffectedDevices() {
  return { mTarget.device };
}

HatPassthrough::HatPassthrough(
  const HatTarget& target
): HatAction(), mTarget(target) {
}

void HatPassthrough::map(int16_t value) {
  mTarget.device->setHat(mTarget.hat, value);
}

std::set<OutputDevice*> HatPassthrough::getAffectedDevices() {
  return { mTarget.device };
}

} // namespace fredemmott::inputmapping
