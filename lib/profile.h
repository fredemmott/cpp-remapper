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

#include "MappableInput.h"
#include "MappableOutput.h"
#include "MappableDS4Output.h"
#include "MappableVJoyOutput.h"
#include "MappableX360Output.h"
#include "devicespecifier.h"
#include "mapper.h"

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

class Profile final {
 public:
  Profile(const std::vector<DeviceSpecifier>& ids);
  Profile(Profile&& other);
  ~Profile();
  void operator=(const Profile&) = delete;
  MappableInput popInput();
  Mapper* operator->() const;

 private:
  struct Impl;
  std::unique_ptr<Impl> p;
};

// Implementation details to make your eyes bleed.
namespace detail {
std::tuple<> get_devices(Profile*);

template <typename... Ts>
auto get_devices(Profile* p, const DeviceSpecifier& first, Ts... rest) {
  auto device = p->popInput();
  return std::tuple_cat(std::make_tuple(device), get_devices(p, rest...));
}

template <typename... Ts>
auto get_devices(Profile* p, const VJoyID& first, Ts... rest) {
  return std::tuple_cat(
    std::make_tuple(MappableVJoyOutput(first.value)), get_devices(p, rest...));
}

template <typename... Ts>
auto get_devices(Profile* p, const ViGEmX360ID& _first, Ts... rest) {
  return std::tuple_cat(
    std::make_tuple(MappableX360Output()), get_devices(p, rest...));
}

template <typename... Ts>
auto get_devices(Profile* p, const ViGEmDS4ID& _first, Ts... rest) {
  return std::tuple_cat(
    std::make_tuple(MappableDS4Output()), get_devices(p, rest...));
}

void fill_input_ids(std::vector<DeviceSpecifier>&);

template <typename... Rest>
void fill_input_ids(
  std::vector<DeviceSpecifier>& device_ids,
  const OutputID&,
  Rest... rest) {
  fill_input_ids(device_ids, rest...);
}

template <typename... Rest>
void fill_input_ids(
  std::vector<DeviceSpecifier>& device_ids,
  const DeviceSpecifier& first,
  Rest... rest) {
  device_ids.push_back(first);
  fill_input_ids(device_ids, rest...);
}

using OutputPtr = std::shared_ptr<OutputDevice>;

std::vector<OutputPtr> select_outputs();

template <typename First, typename... Rest>
std::vector<OutputPtr> select_outputs(const First& first, Rest... rest) {
  auto ret = select_outputs(rest...);
  if constexpr (std::derived_from<First, MappableOutput>) {
    ret.push_back(first.getDevice());
  }
  return ret;
}

std::vector<MappableInput> select_inputs();

template <typename First, typename... Rest>
std::vector<MappableInput> select_inputs(const First& first, Rest... rest) {
  auto ret = select_inputs(rest...);
  if constexpr (std::derived_from<First, MappableInput>) {
    ret.push_back(first);
  }
  return ret;
}

}// namespace detail

template <typename... Ts>
auto create_profile(const DeviceSpecifier& first, Ts... rest) {
  std::vector<DeviceSpecifier> input_ids;
  detail::fill_input_ids(input_ids, first, rest...);
  auto p = Profile(input_ids);
  auto devices = detail::get_devices(&p, first, rest...);
  // Lambda needed as the thing we're calling is a template: std::apply needs
  // an `std::function`, and we can't take a reference to a template function
  p->setDevices(
    std::apply(
      [](auto&&... args) { return detail::select_inputs(args...); }, devices),
    std::apply(
      [](auto&&... args) { return detail::select_outputs(args...); }, devices));
  return std::tuple_cat(std::make_tuple(std::move(p)), devices);
}

}// namespace fredemmott::inputmapping
