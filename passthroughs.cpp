#include "passthroughs.h"

namespace fredemmott::inputmapping {

VJoyAxis::VJoyAxis(
  const AxisTarget& target
): AxisAction(), mTarget(target) {
}

void VJoyAxis::map(long value) {
  mTarget.set(value);
}

VJoyButton::VJoyButton(
  const ButtonTarget& target
): ButtonAction(), mTarget(target) {
}

void VJoyButton::map(bool value) {
  mTarget.set(value);
}

VJoyHat::VJoyHat(
  const HatTarget& target
): HatAction(), mTarget(target) {
}

void VJoyHat::map(int16_t value) {
  mTarget.set(value);
}

} // namespace fredemmott::inputmapping
