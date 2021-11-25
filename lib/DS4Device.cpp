/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "DS4Device.h"

#include <iostream>

#include "Controls.h"
#include "ViGEmClient.h"
#include "ViGEmTarget.h"

namespace fredemmott::inputmapping {

class DS4Device::Impl final
  : public detail::ViGEmTarget<DS4_REPORT, DS4Device::Impl> {
 public:
  static void initState(DS4_REPORT* state) {
    DS4_REPORT_INIT(state);
  }

  static PVIGEM_TARGET allocTarget() {
    return vigem_target_ds4_alloc();
  }

  static const char* productShortName() {
    return "DS4 pad";
  }
};

DS4Device::DS4Device() : p(new Impl()) {
  if (p) {
    std::cout << "Attached ViGEm DS4 pad." << std::endl;
    return;
  }

  std::cerr << "---\nERROR\n---\nFailed to initialize ViGEm DS4 pad."
            << std::endl;
  exit(1);
}

DS4Device::~DS4Device() {
}

void DS4Device::flush() {
  auto& client = ViGEmClient::get();
  if (!client) {
    return;
  }
  vigem_target_ds4_update(client, p->pad, p->state);
}

DS4Device* DS4Device::setButton(Button button, bool value) {
  if (value) {
    p->state.wButtons |= (USHORT)button;
  } else {
    p->state.wButtons ^= (USHORT)button;
  }
  return this;
}

DS4Device* DS4Device::setButton(SpecialButton button, bool value) {
  if (value) {
    p->state.bSpecial |= (BYTE)button;
  } else {
    p->state.bSpecial ^= (BYTE)button;
  }
  return this;
}

DS4Device* DS4Device::setDPad(DPadDirection dpad) {
  DS4_SET_DPAD(&p->state, (DS4_DPAD_DIRECTIONS) dpad);
  return this;
}

namespace {
  inline BYTE scale_axis(long value) {
    return (value * 255) / 65535;
  }
}

DS4Device* DS4Device::setLXAxis(long value) {
  p->state.bThumbLX = scale_axis(value);
  return this;
}
DS4Device* DS4Device::setLYAxis(long value) {
  p->state.bThumbLY = scale_axis(value);
  return this;
}
DS4Device* DS4Device::setRXAxis(long value) {
  p->state.bThumbRX = scale_axis(value);
  return this;
}
DS4Device* DS4Device::setRYAxis(long value) {
  p->state.bThumbRY = scale_axis(value);
  return this;
}

DS4Device* DS4Device::setLTrigger(long value) {
  p->state.bTriggerL = scale_axis(value);
  return this;
}
DS4Device* DS4Device::setRTrigger(long value) {
  p->state.bTriggerR = scale_axis(value);
  return this;
}

}// namespace fredemmott::inputmapping
