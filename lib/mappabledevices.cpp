/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "mappabledevices.h"

#include "axistypes.h"
#include "inputdevice.h"
#include "vjoypp.h"

using fredemmott::gameinput::AxisType;
using fredemmott::gameinput::InputDevice;
using fredemmott::vjoypp::OutputDevice;

namespace fredemmott::inputmapping {

MappableOutput::MappableOutput(uint8_t vjoy_id): MappableOutput(
    new OutputDevice(vjoy_id)) {}

MappableOutput::MappableOutput(OutputDevice* dev):
#define A(x) x ## Axis { dev, #x, &OutputDevice::set ## x ## Axis }
  A(X), A(Y), A(Z),
  A(RX), A(RY), A(RZ),
#undef A
  Slider { dev, "Slider", &OutputDevice::setSlider},
  Dial { dev, "Dial", &OutputDevice::setDial},
#define B(x) Button ## x { dev, x }
  B(1), B(2), B(3), B(4), B(5), B(6), B(7), B(8),
  B(9), B(10), B(11), B(12), B(13), B(14), B(15), B(16),
  B(17), B(18), B(19), B(20), B(21), B(22), B(23), B(24),
  B(25), B(26), B(27), B(28), B(29), B(30), B(31), B(32),
  B(33), B(34), B(35), B(36), B(37), B(38), B(39), B(40),
  B(41), B(42), B(43), B(44), B(45), B(46), B(47), B(48),
  B(49), B(50), B(51), B(52), B(53), B(54), B(55), B(56),
  B(57), B(58), B(59), B(60), B(61), B(62), B(63), B(64),
  B(65), B(66), B(67), B(68), B(69), B(70), B(71), B(72),
  B(73), B(74), B(75), B(76), B(77), B(78), B(79), B(80),
  B(81), B(82), B(83), B(84), B(85), B(86), B(87), B(88),
  B(89), B(90), B(91), B(92), B(93), B(94), B(95), B(96),
  B(97), B(98), B(99), B(100), B(101), B(102), B(103), B(104),
  B(105), B(106), B(107), B(108), B(109), B(110), B(111), B(112),
  B(113), B(114), B(115), B(116), B(117), B(118), B(119), B(120),
  B(121), B(122), B(123), B(124), B(125), B(126), B(127), B(128),
#undef B
#define H(x) Hat ## x { dev, x }
	H(1), H(2), H(3), H(4),
#undef H
  mDevice(dev)
{
}

OutputDevice* MappableOutput::getDevice() const {
  return mDevice;
}

ButtonTarget MappableOutput::button(uint8_t id) const {
  return { mDevice, id };
}

HatTarget MappableOutput::hat(uint8_t id) const {
  return { mDevice, id };
}

namespace {
  AxisSource find_axis(InputDevice* device, AxisType t) {
    const auto info = device->getAxisInformation();
    for (uint8_t i = 0; i < info.size(); ++i) {
      if (info[i].type == t) {
        return {device, (uint8_t) (i + 1) };
      }
    }
    return {nullptr, 0};
  }
} // namespace

MappableInput::MappableInput(InputDevice* dev):
  AxisCount(dev->getAxisCount()),
  ButtonCount(dev->getButtonCount()),
  HatCount(dev->getHatCount()),
#define A(x) x ## Axis(find_axis(dev, AxisType:: ## x ))
  A(X), A(Y), A(Z),
  A(RX), A(RY), A(RZ),
#undef A
  Slider(find_axis(dev, AxisType::SLIDER)),
#define B(x) Button ## x { dev, x }
  B(1), B(2), B(3), B(4), B(5), B(6), B(7), B(8),
  B(9), B(10), B(11), B(12), B(13), B(14), B(15), B(16),
  B(17), B(18), B(19), B(20), B(21), B(22), B(23), B(24),
  B(25), B(26), B(27), B(28), B(29), B(30), B(31), B(32),
  B(33), B(34), B(35), B(36), B(37), B(38), B(39), B(40),
  B(41), B(42), B(43), B(44), B(45), B(46), B(47), B(48),
  B(49), B(50), B(51), B(52), B(53), B(54), B(55), B(56),
  B(57), B(58), B(59), B(60), B(61), B(62), B(63), B(64),
  B(65), B(66), B(67), B(68), B(69), B(70), B(71), B(72),
  B(73), B(74), B(75), B(76), B(77), B(78), B(79), B(80),
  B(81), B(82), B(83), B(84), B(85), B(86), B(87), B(88),
  B(89), B(90), B(91), B(92), B(93), B(94), B(95), B(96),
  B(97), B(98), B(99), B(100), B(101), B(102), B(103), B(104),
  B(105), B(106), B(107), B(108), B(109), B(110), B(111), B(112),
  B(113), B(114), B(115), B(116), B(117), B(118), B(119), B(120),
  B(121), B(122), B(123), B(124), B(125), B(126), B(127), B(128),
#undef B
#define H(x) Hat ## x { dev, x }
	H(1), H(2), H(3), H(4),
#undef H
  mDevice(dev)
{
}

InputDevice* MappableInput::getDevice() const {
  return mDevice;
}

AxisSource MappableInput::axis(uint8_t id) const {
  return { mDevice, id };
}

ButtonSource MappableInput::button(uint8_t id) const {
  return { mDevice, id };
}

HatSource MappableInput::hat(uint8_t id) const {
  return { mDevice, id };
}

} // namespace fredemmott::inputmapping
