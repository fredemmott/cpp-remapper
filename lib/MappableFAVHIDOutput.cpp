/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <cpp-remapper/AxisInformation.h>
#include <cpp-remapper/FAVHIDDevice.h>
#include <cpp-remapper/InputDevice.h>
#include <cpp-remapper/MappableFAVHIDOutput.h>

namespace fredemmott::inputmapping {

struct MappableFAVHIDOutput::Impl final {
  std::shared_ptr<FAVHIDDevice> mDevice;
  FAVHID::FAVJoyState2::Report mState {};
};

MappableFAVHIDOutput::MappableFAVHIDOutput(uint8_t favhid_id)
  : MappableFAVHIDOutput(std::make_shared<FAVHIDDevice>(favhid_id)) {
}

static constexpr decltype(FAVHID::FAVJoyState2::Report::x) ConvertAxisValue(
  Axis::Value input) {
  int64_t value = input;
  value -= Axis::MIN;

  constexpr auto inputRange = Axis::MAX - Axis::MIN;
  using outputLimits
    = std::numeric_limits<decltype(FAVHID::FAVJoyState2::Report::x)>;
  constexpr auto outputRange = outputLimits::max() - outputLimits::min();

  value *= outputRange;
  value /= inputRange;

  value += outputLimits::min();

  return value;
}

MappableFAVHIDOutput::MappableFAVHIDOutput(std::shared_ptr<FAVHIDDevice> dev)
  : p(new Impl {dev}),
#define A(myMember, reportMember) \
  myMember([this](long value) { \
    p->mState.reportMember = ConvertAxisValue(value); \
    p->mDevice->set(p->mState); \
  })
#define AA(myMember, reportMember) A(myMember##Axis, reportMember)
    AA(X, x),
    AA(Y, y),
    AA(Z, z),
    AA(RX, rx),
    AA(RY, ry),
    AA(RZ, rz),
    A(Slider, slider[0]),
    A(Dial, slider[1]),
#undef AA
#undef A
#define B(n) Button##n(button(n))
    B(1),
    B(2),
    B(3),
    B(4),
    B(5),
    B(6),
    B(7),
    B(8),
    B(9),
    B(10),
    B(11),
    B(12),
    B(13),
    B(14),
    B(15),
    B(16),
    B(17),
    B(18),
    B(19),
    B(20),
    B(21),
    B(22),
    B(23),
    B(24),
    B(25),
    B(26),
    B(27),
    B(28),
    B(29),
    B(30),
    B(31),
    B(32),
    B(33),
    B(34),
    B(35),
    B(36),
    B(37),
    B(38),
    B(39),
    B(40),
    B(41),
    B(42),
    B(43),
    B(44),
    B(45),
    B(46),
    B(47),
    B(48),
    B(49),
    B(50),
    B(51),
    B(52),
    B(53),
    B(54),
    B(55),
    B(56),
    B(57),
    B(58),
    B(59),
    B(60),
    B(61),
    B(62),
    B(63),
    B(64),
    B(65),
    B(66),
    B(67),
    B(68),
    B(69),
    B(70),
    B(71),
    B(72),
    B(73),
    B(74),
    B(75),
    B(76),
    B(77),
    B(78),
    B(79),
    B(80),
    B(81),
    B(82),
    B(83),
    B(84),
    B(85),
    B(86),
    B(87),
    B(88),
    B(89),
    B(90),
    B(91),
    B(92),
    B(93),
    B(94),
    B(95),
    B(96),
    B(97),
    B(98),
    B(99),
    B(100),
    B(101),
    B(102),
    B(103),
    B(104),
    B(105),
    B(106),
    B(107),
    B(108),
    B(109),
    B(110),
    B(111),
    B(112),
    B(113),
    B(114),
    B(115),
    B(116),
    B(117),
    B(118),
    B(119),
    B(120),
    B(121),
    B(122),
    B(123),
    B(124),
    B(125),
    B(126),
    B(127),
    B(128),
#undef B
#define H(n) Hat##n(hat(n))
    H(1),
    H(2),
    H(3),
    H(4)
#undef H
{
}

MappableFAVHIDOutput::~MappableFAVHIDOutput() {
}

std::shared_ptr<OutputDevice> MappableFAVHIDOutput::getDevice() const {
  if (!p) {
    return {};
  }

  return p->mDevice;
}

ButtonSinkPtr MappableFAVHIDOutput::button(uint8_t id) const {
  return [this, id](Button::Value value) {
    p->mState.SetButton(id - 1, value);
    p->mDevice->set(p->mState);
  };
}

static constexpr int8_t ConvertHatValue(Hat::Value value) {
  if (value == Hat::CENTER) {
    return 0b1111;
  }
  return value / 4500;
}

HatSinkPtr MappableFAVHIDOutput::hat(uint8_t id) const {
  return [this, id](Hat::Value value) {
    p->mState.SetPOV(id - 1, ConvertHatValue(value));
    p->mDevice->set(p->mState);
  };
}

}// namespace fredemmott::inputmapping
