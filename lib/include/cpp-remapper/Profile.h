/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cpp-remapper/DeviceSpecifier.h>
#include <cpp-remapper/EventLoop.h>
#include <cpp-remapper/InputDeviceCollection.h>
#include <cpp-remapper/MappableDS4Output.h>
#include <cpp-remapper/MappableFAVHIDOutput.h>
#include <cpp-remapper/MappableInput.h>
#include <cpp-remapper/MappableOutput.h>
#include <cpp-remapper/MappableVJoyOutput.h>
#include <cpp-remapper/MappableX360Output.h>
#include <cpp-remapper/OutputDevice.h>

#include <memory>
#include <tuple>
#include <vector>

namespace fredemmott::inputmapping {

namespace detail {
struct OutputID {};
struct ViGEmX360ID : public OutputID {};
struct ViGEmDS4ID : public OutputID {};

struct VJoyID : public OutputID {
  VJoyID() = delete;
  VJoyID(uint8_t value) : value(value) {
  }

  const uint8_t value;
};

struct FAVHIDID : public OutputID {
  FAVHIDID() = delete;

  FAVHIDID(uint8_t value) : value(value) {
  }

  const uint8_t value;
};

}// namespace detail

const detail::VJoyID VJOY_1 {1}, VJOY_2 {2}, VJOY_3 {3}, VJOY_4 {4}, VJOY_5 {5},
  VJOY_6 {6}, VJOY_7 {7}, VJOY_8 {8}, VJOY_9 {9}, VJOY_10 {10}, VJOY_11 {11},
  VJOY_12 {12}, VJOY_13 {13}, VJOY_14 {14}, VJOY_15 {15}, VJOY_16 {16};

const detail::FAVHIDID FAVHID_1 {0}, FAVHID_2 {1}, FAVHID_3 {2}, FAVHID_4 {3},
  FAVHID_5 {4}, FAVHID_6 {5}, FAVHID_7 {6}, FAVHID_8 {7};

const detail::ViGEmX360ID VIGEM_X360_PAD;
const detail::ViGEmDS4ID VIGEM_DS4_PAD;

class DeviceWithVisibility {
 private:
  DeviceSpecifier impl;

 public:
  explicit DeviceWithVisibility(const DeviceSpecifier& ds);
  DeviceSpecifier getSpecifier() const;
};

class UnhiddenDevice final : public DeviceWithVisibility {
 public:
  using DeviceWithVisibility::DeviceWithVisibility;
};

class HiddenDevice final : public DeviceWithVisibility {
 public:
  // clang-format off
    template<std::convertible_to<DeviceSpecifier> T>
    requires (!std::convertible_to<T, UnhiddenDevice>)
    explicit HiddenDevice(const T& ds): DeviceWithVisibility(DeviceSpecifier(ds)) {}
  // clang-format on
  HiddenDevice(const UnhiddenDevice&) = delete;
};

class Profile final {
 public:
  Profile(const std::vector<HiddenDevice>& hidden_ids);
  Profile(Profile&& other);
  ~Profile();
  void operator=(const Profile&) = delete;

  std::shared_ptr<EventLoop> getEventLoop() const;
  void run();

 private:
  struct Impl;
  std::unique_ptr<Impl> p;
};

// Implementation details to make your eyes bleed.
namespace detail {
std::tuple<> get_devices(Profile*, InputDeviceCollection*);

MappableInput get_device(InputDeviceCollection*, const DeviceSpecifier&);

template <typename... Ts>
auto get_devices(
  Profile* p,
  InputDeviceCollection* c,
  const DeviceSpecifier& first,
  Ts... rest) {
  auto device = get_device(c, first);
  return std::tuple_cat(std::make_tuple(device), get_devices(p, c, rest...));
}

template <typename... Ts>
auto get_devices(
  Profile* p,
  InputDeviceCollection* c,
  const DeviceWithVisibility& first,
  Ts... rest) {
  auto device = get_device(c, first.getSpecifier());
  return std::tuple_cat(std::make_tuple(device), get_devices(p, c, rest...));
}

template <typename... Ts>
auto get_devices(
  Profile* p,
  InputDeviceCollection* c,
  const VJoyID& first,
  Ts... rest) {
  return std::tuple_cat(
    std::make_tuple(MappableVJoyOutput(first.value)),
    get_devices(p, c, rest...));
}

template <typename... Ts>
auto get_devices(
  Profile* p,
  InputDeviceCollection* c,
  const FAVHIDID& first,
  Ts... rest) {
  return std::tuple_cat(
    std::make_tuple(MappableFAVHIDOutput(first.value)),
    get_devices(p, c, rest...));
}

template <typename... Ts>
auto get_devices(
  Profile* p,
  InputDeviceCollection* c,
  const ViGEmX360ID& _first,
  Ts... rest) {
  return std::tuple_cat(
    std::make_tuple(MappableX360Output()), get_devices(p, c, rest...));
}

template <typename... Ts>
auto get_devices(
  Profile* p,
  InputDeviceCollection* c,
  const ViGEmDS4ID& _first,
  Ts... rest) {
  return std::tuple_cat(
    std::make_tuple(MappableDS4Output()), get_devices(p, c, rest...));
}

void fill_hidden_ids(std::vector<HiddenDevice>&);

template <typename First, typename... Rest>
void fill_hidden_ids(
  std::vector<HiddenDevice>& hidden_devices,
  const First& first,
  Rest... rest) {
  if constexpr (std::is_constructible_v<HiddenDevice, First>) {
    hidden_devices.push_back(HiddenDevice {first});
  }

  fill_hidden_ids(hidden_devices, rest...);
}

std::vector<std::shared_ptr<EventSink>> get_event_sinks();

template <typename First, typename... Rest>
std::vector<std::shared_ptr<EventSink>> get_event_sinks(
  const First& first,
  Rest... rest) {
  auto ret = get_event_sinks(rest...);
  if constexpr (std::derived_from<First, MappableOutput>) {
    ret.push_back(first.getDevice());
  }
  return ret;
}

std::vector<std::shared_ptr<EventSource>> get_event_sources();

template <typename First, typename... Rest>
std::vector<std::shared_ptr<EventSource>> get_event_sources(
  const First& first,
  Rest... rest) {
  auto ret = get_event_sources(rest...);
  if constexpr (std::convertible_to<First, MappableInput>) {
    ret.push_back(first.getEventSource());
  }
  return ret;
}

}// namespace detail

template <typename... Ts>
auto create_profile(Ts... specifiers) {
  std::vector<HiddenDevice> input_ids;
  detail::fill_hidden_ids(input_ids, specifiers...);

  auto p = Profile(input_ids);

  InputDeviceCollection device_collection;
  auto devices = detail::get_devices(&p, &device_collection, specifiers...);
  // Lambda needed as the thing we're calling is a template: std::apply needs
  // an `std::function`, and we can't take a reference to a template function
  auto event_loop = p.getEventLoop();
  event_loop->setEventSources(std::apply(
    [](auto&&... args) { return detail::get_event_sources(args...); },
    devices));
  event_loop->setEventSinks(std::apply(
    [](auto&&... args) { return detail::get_event_sinks(args...); }, devices));
  return std::tuple_cat(std::make_tuple(std::move(p)), devices);
}

}// namespace fredemmott::inputmapping
