/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <cpp-remapper/AxisInformation.h>
#include <cpp-remapper/InputDevice.h>
#include <cpp-remapper/MappableX360Output.h>
#include <cpp-remapper/X360Device.h>

namespace fredemmott::inputmapping {

MappableX360Output::MappableX360Output()
  : MappableX360Output(std::make_shared<X360Device>()) {
}

MappableX360Output::MappableX360Output(std::shared_ptr<X360Device> dev)
  :
#define A(a) a([dev](long value) { dev->set##a(value); })
#define AA(a) A(a##Axis)
    AA(LX),
    AA(LY),
    A(LTrigger),
    AA(RX),
    AA(RY),
    A(RTrigger),
#undef AA
#undef A
#define B(name, enum) \
  name([dev](bool value) { dev->setButton(X360Device::Button::enum, value); })
#define BB(x) B(Button##x, x)
    BB(A),
    BB(B),
    BB(X),
    BB(Y),
#undef BB
#define BB(name, enum) B(Button##name, enum)
    BB(Back, BACK),
    BB(Start, START),
    BB(Guide, GUIDE),
    B(DPadUp, DPAD_UP),
    B(DPadDown, DPAD_DOWN),
    B(DPadLeft, DPAD_LEFT),
    B(DPadRight, DPAD_RIGHT),
    BB(LStick, LEFT_STICK),
    BB(RStick, RIGHT_STICK),
    BB(LShoulder, LEFT_SHOULDER),
    BB(RShoulder, RIGHT_SHOULDER),
#undef BB
#undef B
    mDevice(dev) {
}

MappableX360Output::~MappableX360Output() {
}

std::shared_ptr<OutputDevice> MappableX360Output::getDevice() const {
  return mDevice;
}

}// namespace fredemmott::inputmapping
