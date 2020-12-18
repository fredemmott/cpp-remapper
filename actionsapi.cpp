#pragma once

#include "actionsapi.h"

#include "vjoypp.h"

namespace fredemmott::inputmapping {

void ButtonTarget::set(bool value) const {
  device->setButton(button, value);
}

void HatTarget::set(int16_t value) const {
  device->setHat(hat, value);
}

void AxisTarget::set(long value) const {
  (device->*(setter))(value);
}

} // namespace fredemmott::inputmapping
