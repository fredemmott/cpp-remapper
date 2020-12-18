#pragma once

#include "actionsapi.h"

#include "vjoypp.h"

namespace fredemmott::inputmapping {

void ButtonTarget::set(bool value) {
  device->setButton(button, value);
}

void HatTarget::set(int16_t value) {
  device->setHat(hat, value);
}

void AxisTarget::set(long value) {
  (device->*(setter))(value);
}

} // namespace fredemmott::inputmapping
