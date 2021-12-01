/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "VJoyDevice.h"

#include <cassert>
#include <cstdio>
#include <stdexcept>

// clang-format off
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "SDK/inc/public.h"
#include "SDK/inc/vjoyinterface.h"
// clang-format on

#pragma comment(lib, "vJoyInterface.lib")

namespace fredemmott::inputmapping {
namespace {
bool vjoy_initialized = false;

void init_vjoy() {
  if (vjoy_initialized) {
    return;
  }
  vjoy_initialized = true;

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
      VerDll);
  }
}
}// namespace

struct VJoyDevice::Impl {
  BYTE id;
#if USE_JOYSTICK_API_VERSION == 3
  JOYSTICK_POSITION_V3 state;
#elif USE_JOYSTICK_API_VERSION == 2
  JOYSTICK_POSITION_V2 state;
#endif
};

VJoyDevice::VJoyDevice(uint8_t id) : p(new Impl {id, {}}) {
  init_vjoy();
  AcquireVJD(id);
  ResetVJD(id);
  setHat(1, 0xffff);
  setHat(2, 0xffff);
  setHat(3, 0xffff);
  setHat(4, 0xffff);
}

VJoyDevice::~VJoyDevice() {
  RelinquishVJD(p->id);
}

namespace {
/* DX gives 0->0xffff
 * VJoy wants 1 -> 0x8000
 */
long normalize_axis(long dx) {
  return ((dx * (0x8000 - 1)) / 0xffff) + 1;
}
}// namespace

VJoyDevice *VJoyDevice::setXAxis(long value) {
  p->state.wAxisX = normalize_axis(value);
  return this;
}

VJoyDevice *VJoyDevice::setYAxis(long value) {
  p->state.wAxisY = normalize_axis(value);
  return this;
}

VJoyDevice *VJoyDevice::setZAxis(long value) {
  p->state.wAxisZ = normalize_axis(value);
  return this;
}

VJoyDevice *VJoyDevice::setRXAxis(long value) {
  p->state.wAxisXRot = normalize_axis(value);
  return this;
}

VJoyDevice *VJoyDevice::setRYAxis(long value) {
  p->state.wAxisYRot = normalize_axis(value);
  return this;
}

VJoyDevice *VJoyDevice::setRZAxis(long value) {
  p->state.wAxisZRot = normalize_axis(value);
  return this;
}

VJoyDevice *VJoyDevice::setSlider(long value) {
  p->state.wSlider = normalize_axis(value);
  return this;
}

VJoyDevice *VJoyDevice::setDial(long value) {
  p->state.wDial = normalize_axis(value);
  return this;
}

VJoyDevice *VJoyDevice::setButton(uint8_t button, bool value) {
  button--;
  off_t offset = button % 32;
  auto mask = 1 << offset;

  LONG *data = nullptr;
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

VJoyDevice *VJoyDevice::setHat(uint8_t hat, uint16_t v) {
  const long value = v == 0xffff ? -1 : v;
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

void VJoyDevice::flush() {
  UpdateVJD(p->id, &p->state);
}

}// namespace fredemmott::inputmapping
