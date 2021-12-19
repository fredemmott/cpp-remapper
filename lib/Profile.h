/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "DeviceSpecifier.h"
#include "InputDeviceCollection.h"
#include "MappableDS4Output.h"
#include "MappableInput.h"
#include "MappableOutput.h"
#include "MappableVJoyOutput.h"
#include "MappableX360Output.h"
#include "Mapper.h"
#include "OutputDevice.h"

namespace fredemmott::inputmapping {

namespace detail {
struct OutputID {};
struct ViGEmX360ID : public OutputID {};
struct ViGEmDS4ID : public OutputID {};

struct VJoyID : public OutputID {
  VJoyID(uint8_t value) : value(value) {
  }

  const uint8_t value;
};

}// namespace detail

const detail::VJoyID VJOY_1 {1}, VJOY_2 {2}, VJOY_3 {3}, VJOY_4 {4}, VJOY_5 {5},
  VJOY_6 {6}, VJOY_7 {7}, VJOY_8 {8}, VJOY_9 {9}, VJOY_10 {10}, VJOY_11 {11},
  VJOY_12 {12}, VJOY_13 {13}, VJOY_14 {14}, VJOY_15 {15}, VJOY_16 {16};

const detail::ViGEmX360ID VIGEM_X360_PAD;
const detail::ViGEmDS4ID VIGEM_DS4_PAD;

class DeviceWithVisibility {
 private:
  DeviceSpecifier impl;

 public:
  explicit DeviceWithVisibility(const DeviceSpecifier& ds);
  operator const DeviceSpecifier&() const;
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
    HiddenDevice(const T& ds): DeviceWithVisibility(DeviceSpecifier(ds)) {}
  // clang-format on
};

class Profile final {
 public:
  Profile(const std::vector<HiddenDevice>& hidden_ids);
  Profile(Profile&& other);
  ~Profile();
  void operator=(const Profile&) = delete;

  std::shared_ptr<Mapper> getEventLoop() const;
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
  if constexpr (std::convertible_to<First, HiddenDevice>) {
    hidden_devices.push_back(first);
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
auto create_profile(const DeviceSpecifier& first, Ts... rest) {
  std::vector<HiddenDevice> input_ids;
  detail::fill_hidden_ids(input_ids, first, rest...);

  auto p = Profile(input_ids);

  InputDeviceCollection device_collection;
  auto devices = detail::get_devices(&p, &device_collection, first, rest...);
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
