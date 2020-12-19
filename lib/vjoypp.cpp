/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "vjoypp.h"

#include <cassert>
#include <cstdio>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "SDK/inc/public.h"
#include "SDK/inc/vjoyinterface.h"

#pragma comment(lib, "SDK/lib/vJoyInterface.lib")

namespace fredemmott::vjoypp {

void init() {
  if (!vJoyEnabled()) {
    printf("vJoy is not enabled.\n");
    return;
  }
  WORD VerDll, VerDrv;
  if (!DriverMatch(&VerDll, &VerDrv)) {
    printf(
      "---\n"
      "!!! WARNING !!!\n"
      "vJoy driver version %04x does not match DLL version %04x\n"
      "---\n",
      VerDrv,
      VerDll
    );
  }
}

struct OutputDevice::Impl {
  BYTE id;
#if USE_JOYSTICK_API_VERSION == 3
  JOYSTICK_POSITION_V3 state;
#elif USE_JOYSTICK_API_VERSION == 2
  JOYSTICK_POSITION_V2 state;
#endif
};

OutputDevice::OutputDevice(uint8_t id): p(new Impl { id, {} }) {
  AcquireVJD(id);
  ResetVJD(id);
}

OutputDevice::~OutputDevice() {
  RelinquishVJD(p->id);
}

namespace {
  /* DX gives 0->0xffff
   * VJoy wants 1 -> 0x8000
   */
  long normalize_axis(long dx) {
    return ((dx * (0x8000 - 1)) / 0xffff) + 1;
  }
}

OutputDevice* OutputDevice::setXAxis(long value) {
  p->state.wAxisX = normalize_axis(value);
  return this;
}

OutputDevice* OutputDevice::setYAxis(long value) {
  p->state.wAxisY = normalize_axis(value);
  return this;
}

OutputDevice* OutputDevice::setZAxis(long value) {
  p->state.wAxisZ = normalize_axis(value);
  return this;
}

OutputDevice* OutputDevice::setRXAxis(long value) {
  p->state.wAxisXRot = normalize_axis(value);
  return this;
}

OutputDevice* OutputDevice::setRYAxis(long value) {
  p->state.wAxisYRot = normalize_axis(value);
  return this;
}

OutputDevice* OutputDevice::setRZAxis(long value) {
  p->state.wAxisZRot = normalize_axis(value);
  return this;
}

OutputDevice* OutputDevice::setSlider(long value) {
  p->state.wSlider = normalize_axis(value);
  return this;
}

OutputDevice* OutputDevice::setDial(long value) {
  p->state.wSlider = normalize_axis(value);
  return this;
}

OutputDevice* OutputDevice::setButton(uint8_t button, bool value) {
  button--;
  off_t offset = button % 32;
  auto mask = 1 << offset;

  LONG* data = nullptr;
  if (button < 32) {
    data = &p->state.lButtons;
  } else if (button < 64) {
    data = &p->state.lButtonsEx1;
  } else if (button < 96) {
    data = &p->state.lButtonsEx2;
  } else if (button < 128) {
    data = &p->state.lButtonsEx3;
  }
  assert(data);

  if (value) {
    *data |= mask;
  } else {
    *data &= ~mask;
  }

  return this;
}

OutputDevice* OutputDevice::setHat(uint8_t hat, int16_t value) {
  switch (hat) {
    case 1:
      p->state.bHats = value;
      break;
    case 2:
      p->state.bHatsEx1 = value;
      break;
    case 3:
      p->state.bHatsEx2 = value;
      break;
    case 4:
      p->state.bHatsEx3 = value;
      break;
  }
  return this;
}

void OutputDevice::send() {
  UpdateVJD(p->id, &p->state);
}

} // namespace fredemmott::vjoypp
