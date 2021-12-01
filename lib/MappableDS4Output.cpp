/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "MappableDS4Output.h"

#include "AxisInformation.h"
#include "DS4Device.h"
#include "InputDevice.h"

namespace fredemmott::inputmapping {

MappableDS4Output::MappableDS4Output()
  : MappableDS4Output(std::make_shared<DS4Device>()) {
}

namespace {
void button_to_dpad(
  const std::shared_ptr<DS4Device>& device,
  DS4Device::DPadDirection direction,
  bool value) {
  if (!value) {
    device->setDPad(DS4Device::DPadDirection::NONE);
    return;
  }
  device->setDPad(direction);
}
}// namespace

MappableDS4Output::MappableDS4Output(std::shared_ptr<DS4Device> dev)
  :
#define A(a) a([dev](long value) { dev->set##a(value); })
#define AA(a) A(a##Axis)
    AA(LX),
    AA(LY),
    AA(RX),
    AA(RY),
#undef AA
    A(LTrigger),
    A(RTrigger),
#undef A
#define B(name, enum) \
  Button##name( \
    [dev](bool value) { dev->setButton(DS4Device::Button::enum, value); })
    B(Triangle, TRIANGLE),
    B(Circle, CIRCLE),
    B(Cross, CROSS),
    B(Square, SQUARE),
    B(LTrigger, LEFT_TRIGGER),
    B(RTrigger, RIGHT_TRIGGER),
    B(LShoulder, LEFT_SHOULDER),
    B(RShoulder, RIGHT_SHOULDER),
    B(Options, OPTIONS),
    B(Share, SHARE),
#undef B
#define SB(name, enum) \
  Button##name([dev](bool value) { \
    dev->setButton(DS4Device::SpecialButton::enum, value); \
  })
    SB(PS, PS),
    SB(TouchPad, TOUCHPAD),
#undef SB
#define DP(name, enum) \
  ButtonDPad##name([dev](bool value) { \
    button_to_dpad(dev, DS4Device::DPadDirection::enum, value); \
  })
    DP(North, NORTH),
    DP(NorthEast, NORTHEAST),
    DP(East, EAST),
    DP(SouthEast, SOUTHEAST),
    DP(South, SOUTH),
    DP(SouthWest, SOUTHWEST),
    DP(West, WEST),
    DP(NorthWest, NORTHWEST),
#undef DP
    mDevice(dev) {
}

MappableDS4Output::~MappableDS4Output() {
}

std::shared_ptr<OutputDevice> MappableDS4Output::getDevice() const {
  return mDevice;
}

}// namespace fredemmott::inputmapping
