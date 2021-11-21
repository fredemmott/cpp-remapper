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

#include "mapper.h"
#include "MappableInput.h"
#include "MappableOutput.h"
#include "MappableVJoyOutput.h"
#include "MappableX360Output.h"
#include "devicespecifier.h"

namespace fredemmott::inputmapping {

using fredemmott::gameinput::InputDevice;

class Mapper;
struct MappableInput;

namespace {
struct _OUTPUT_ID_T {};
struct _VIGEM_X360_PAD_T : public _OUTPUT_ID_T {};
}// namespace

struct VJOY_ID : public _OUTPUT_ID_T {
  VJOY_ID(uint8_t value) : value(value) {
  }

  const uint8_t value;
};

class Profile final {
 public:
  Profile(const std::vector<gameinput::DeviceSpecifier>& ids);
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
namespace {
auto get_devices(Profile*) {
  return std::make_tuple();
}

template <typename... Ts>
auto get_devices(
  Profile* p,
  const gameinput::DeviceSpecifier& first,
  Ts... rest) {
  auto device = p->popInput();
  return std::tuple_cat(std::make_tuple(device), get_devices(p, rest...));
}

template <typename... Ts>
auto get_devices(Profile* p, const VJOY_ID& first, Ts... rest) {
  return std::tuple_cat(
    std::make_tuple(MappableVJoyOutput(first.value)), get_devices(p, rest...));
}

template <typename... Ts>
auto get_devices(Profile* p, const _VIGEM_X360_PAD_T& _first, Ts... rest) {
  return std::tuple_cat(
    std::make_tuple(MappableX360Output()), get_devices(p, rest...));
}

void fill_input_ids(std::vector<gameinput::DeviceSpecifier>&) {
}

template <typename... Rest>
void fill_input_ids(
  std::vector<gameinput::DeviceSpecifier>& device_ids,
  const _OUTPUT_ID_T&,
  Rest... rest) {
  fill_input_ids(device_ids, rest...);
}

template <typename... Rest>
void fill_input_ids(
  std::vector<gameinput::DeviceSpecifier>& device_ids,
  const gameinput::DeviceSpecifier& first,
  Rest... rest) {
  device_ids.push_back(first);
  fill_input_ids(device_ids, rest...);
}

using OutputPtr = std::shared_ptr<OutputDevice>;

std::vector<OutputPtr> select_outputs() {
  return {};
}

template <typename... Rest>
std::vector<OutputPtr> select_outputs(
  const MappableInput& _not_an_output,
  Rest... rest) {
  return select_outputs(rest...);
}

template <typename... Rest>
std::vector<OutputPtr> select_outputs(
  const MappableOutput& first,
  Rest... rest) {
  auto ret = select_outputs(rest...);
  ret.push_back(first.getDevice());
  return ret;
}

std::vector<MappableInput> select_inputs() {
  return {};
}

template <typename... Rest>
std::vector<MappableInput> select_inputs(
  const MappableOutput& _not_an_input,
  Rest... rest) {
  return select_inputs(rest...);
}

template <typename... Rest>
std::vector<MappableInput> select_inputs(
  const MappableInput& first,
  Rest... rest) {
  auto ret = select_inputs(rest...);
  ret.push_back(first);
  return ret;
}
}// namespace

template <typename... Ts>
auto create_profile(const gameinput::DeviceSpecifier& first, Ts... rest) {
  std::vector<gameinput::DeviceSpecifier> input_ids;
  fill_input_ids(input_ids, first, rest...);
  auto p = Profile(input_ids);
  auto devices = get_devices(&p, first, rest...);
  // Lambda needed as the thing we're calling is a template: std::apply needs
  // an `std::function`, and we can't take a reference to a template function
  p->setDevices(
    std::apply([](auto&&... args) { return select_inputs(args...); }, devices),
    std::apply(
      [](auto&&... args) { return select_outputs(args...); }, devices));
  return std::tuple_cat(std::make_tuple(std::move(p)), devices);
}

namespace vjoyids {
const VJOY_ID VJOY_1 {1}, VJOY_2 {2}, VJOY_3 {3}, VJOY_4 {4}, VJOY_5 {5},
  VJOY_6 {6}, VJOY_7 {7}, VJOY_8 {8}, VJOY_9 {9}, VJOY_10 {10}, VJOY_11 {11},
  VJOY_12 {12}, VJOY_13 {13}, VJOY_14 {14}, VJOY_15 {15}, VJOY_16 {16};
}

namespace vigemids {
const _VIGEM_X360_PAD_T VIGEM_X360_PAD_1;
}

}// namespace fredemmott::inputmapping
