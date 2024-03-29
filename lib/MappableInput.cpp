/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <cpp-remapper/AxisInformation.h>
#include <cpp-remapper/EventSource.h>
#include <cpp-remapper/InputDevice.h>
#include <cpp-remapper/MappableInput.h>

#include <format>

namespace fredemmott::inputmapping {

namespace {
template <class Source>
class MISource : public Source {
 public:
  using Source::emit;
};

template <typename TControl>
class MissingSource final : public Source<TControl> {
  std::string mDevice;
  std::string mControl;

 public:
  MissingSource(const std::string& device, const std::string& control)
    : mDevice(device), mControl(control) {
  }

  void setNext(const maybe_shared_ptr<Sink<TControl>>&) override {
    printf(
      "WARNING: Attempted to attach to '%s', but that does not exist on '%s'. "
      "Ignoring.\n",
      mControl.c_str(),
      mDevice.c_str());
  }
};

class MIAxisSource final : public MISource<AxisSource> {};
class MIButtonSource final : public MISource<ButtonSource> {};
class MIHatSource final : public MISource<HatSource> {};

template <typename TSource>
std::vector<std::shared_ptr<TSource>> fill_sources(size_t count) {
  std::vector<std::shared_ptr<TSource>> ret;
  for (auto i = 0; i < count; ++i) {
    ret.push_back(std::make_shared<TSource>());
  }
  return ret;
}

AxisSourcePtr find_axis(
  const std::shared_ptr<InputDevice>& device,
  std::vector<std::shared_ptr<MIAxisSource>> inputs,
  AxisType t,
  uint8_t skip = 0) {
  const auto skip_in = skip;

  const auto info = device->getAxisInformation();
  for (uint8_t i = 0; i < info.size(); ++i) {
    if (info[i].type == t) {
      if (skip == 0) {
        return inputs.at(i);
      }
      --skip;
    }
  }
  auto axis_name = AxisInformation(t).name;
  if (skip_in) {
    axis_name += std::format("[{}]", skip_in);
  }
  auto product_name = device->getProductName();
  return std::make_shared<MissingSource<Axis>>(product_name, axis_name);
}
}// namespace

class MappableInput::Impl : public EventSource {
 public:
  std::shared_ptr<InputDevice> device;
  InputDevice::State state;
  std::vector<std::shared_ptr<MIAxisSource>> axisInputs;
  std::vector<std::shared_ptr<MIButtonSource>> buttonInputs;
  std::vector<std::shared_ptr<MIHatSource>> hatInputs;
  Impl() = delete;
  Impl(const std::shared_ptr<InputDevice>& dev)
    : device(dev),
      state(dev->getState()),
      axisInputs(fill_sources<MIAxisSource>(dev->getAxisCount())),
      buttonInputs(fill_sources<MIButtonSource>(dev->getButtonCount())),
      hatInputs(fill_sources<MIHatSource>(dev->getAxisCount())) {
  }

  virtual HANDLE getHandle() override {
    return device->getEvent();
  }

  virtual void poll() override;
};

MappableInput::MappableInput(const std::shared_ptr<InputDevice>& dev)
  : p(std::make_shared<Impl>(dev)),
#define A(x) x##Axis(find_axis(dev, p->axisInputs, AxisType::x))
    A(X),
    A(Y),
    A(Z),
    A(RX),
    A(RY),
    A(RZ),
#undef A
    Slider(find_axis(dev, p->axisInputs, AxisType::SLIDER)),
    Dial(find_axis(dev, p->axisInputs, AxisType::SLIDER, 1)),
#define B(x) Button##x(button(x))
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
#define H(x) Hat##x(hat(x))
    H(1),
    H(2),
    H(3),
    H(4)
#undef H
{
}

MappableInput::~MappableInput() {
}

size_t MappableInput::getAxisCount() const {
  return p->device->getAxisCount();
}

size_t MappableInput::getButtonCount() const {
  return p->device->getButtonCount();
}

size_t MappableInput::getHatCount() const {
  return p->device->getHatCount();
}

std::shared_ptr<EventSource> MappableInput::getEventSource() const {
  return p;
}

AxisSourcePtr MappableInput::axis(uint8_t id) const {
  if (id == 0 || id > getAxisCount()) {
    char buf[255];
    snprintf(buf, sizeof(buf), "axis number %d of %zu", id, getAxisCount());
    return std::make_shared<MissingSource<Axis>>(
      p->device->getProductName(), buf);
  }

  return p->axisInputs.at(id - 1);
}

ButtonSourcePtr MappableInput::button(uint8_t id) const {
  if (id == 0 || id > getButtonCount()) {
    char buf[255];
    // TODO: snprintf -> std::format
    snprintf(buf, sizeof(buf), "button number %d of %zu", id, getButtonCount());
    return std::make_shared<MissingSource<Button>>(
      p->device->getProductName(), buf);
  }

  return p->buttonInputs.at(id - 1);
}

HatSourcePtr MappableInput::hat(uint8_t id) const {
  if (id == 0 || id > getHatCount()) {
    char buf[255];
    snprintf(buf, sizeof(buf), "hat number %d of %zu", id, getHatCount());
    return std::make_shared<MissingSource<Hat>>(
      p->device->getProductName(), buf);
  }
  return p->hatInputs.at(id - 1);
}

void MappableInput::Impl::poll() {
  auto a = state;
  auto b = device->getState();
  state = b;

#ifdef VERBOSE_INPUT_DEBUG
  printf("-");
  a.dump();
  printf("+");
  b.dump();
#endif

  const auto axes = axisInputs.size(), buttons = buttonInputs.size(),
             hats = hatInputs.size();

  for (auto i = 0; i < axes; ++i) {
    if (a.getAxis(i) != b.getAxis(i)) {
      axisInputs[i]->emit(b.getAxis(i));
    }
  }

  for (auto i = 0; i < buttons; ++i) {
    if (a.getButton(i) != b.getButton(i)) {
      buttonInputs[i]->emit(b.getButton(i));
    }
  }

  for (auto i = 0; i < hats; ++i) {
    if (a.getHat(i) != b.getHat(i)) {
      hatInputs[i]->emit(b.getHat(i));
    }
  }
}

}// namespace fredemmott::inputmapping
