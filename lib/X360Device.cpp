/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "X360Device.h"

#include <iostream>

#include "ViGEmClient.h"
#include "ViGEmTarget.h"

namespace fredemmott::inputmapping {

class X360Device::Impl final
  : public detail::ViGEmTarget<XUSB_REPORT, X360Device::Impl> {
 public:
  static void initState(XUSB_REPORT* state) {
    XUSB_REPORT_INIT(state);
  }

  static PVIGEM_TARGET allocTarget() {
    return vigem_target_x360_alloc();
  }

  static const char* productShortName() {
    return "X360 pad";
  }
};

X360Device::X360Device() : p(new Impl()) {
  if (p) {
    std::cout << "Attached ViGEm X360 pad." << std::endl;
    return;
  }

  std::cerr << "---\nERROR\n---\nFailed to initialize ViGEm X360 pad."
            << std::endl;
  exit(1);
}

X360Device::~X360Device() {
}

void X360Device::flush() {
  auto& client = ViGEmClient::get();
  if (!client) {
    return;
  }
  vigem_target_x360_update(client, p->pad, p->state);
}

X360Device* X360Device::setButton(Button button, bool value) {
  if (value) {
    p->state.wButtons |= static_cast<USHORT>(button);
  } else {
    p->state.wButtons &= ~static_cast<USHORT>(button);
  }
  return this;
}

X360Device* X360Device::setLXAxis(long value) {
  p->state.sThumbLX = value - 32768;
  return this;
}
X360Device* X360Device::setLYAxis(long value) {
  p->state.sThumbLY = 32767 - value;
  return this;
}
X360Device* X360Device::setRXAxis(long value) {
  p->state.sThumbRX = value - 32768;
  return this;
}
X360Device* X360Device::setRYAxis(long value) {
  p->state.sThumbRY = 32767 - value;
  return this;
}

X360Device* X360Device::setLTrigger(long value) {
  p->state.bLeftTrigger = (value * 255) / 65535;
  return this;
}
X360Device* X360Device::setRTrigger(long value) {
  p->state.bRightTrigger = (value * 255) / 65535;
  return this;
}

}// namespace fredemmott::inputmapping
