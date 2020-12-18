#pragma once

#include "mappabledevices.h"

#include <memory>
#include <vector>

namespace fredemmott::gameinput {
  struct DeviceID;
}

namespace fredemmott::inputmapping {

class Mapper;
struct MappableInput;

struct VJOY_ID {
  const uint8_t value;
};

class Profile final {
 public:
   Profile(const std::vector<gameinput::DeviceID>& ids);
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

  template<typename... Ts>
  auto get_devices(Profile* p, const gameinput::DeviceID& first, Ts... rest) {
    auto device = p->popInput();
    return std::tuple_cat(
      std::make_tuple(device),
      get_devices(p, rest...)
    );
  }

  template<typename... Ts>
  auto get_devices(Profile* p, const VJOY_ID& first, Ts...rest) {
    return std::tuple_cat(
      std::make_tuple(MappableOutput(first.value)),
      get_devices(p, rest...)
    );
  }


  void fill_input_ids(std::vector<gameinput::DeviceID>&) {}

  template<typename...Rest>
  void fill_input_ids(
    std::vector<gameinput::DeviceID>& device_ids,
    const VJOY_ID&,
    Rest... rest
  ) {
    fill_input_ids(device_ids, rest...);
  }

  template<typename...Rest>
  void fill_input_ids(
    std::vector<gameinput::DeviceID>& device_ids,
    const gameinput::DeviceID& first, Rest... rest
  ) {
    device_ids.push_back(first);
    fill_input_ids(device_ids, rest...);
  }
}

template<typename... Ts>
auto create_profile(const gameinput::DeviceID& first, Ts... rest) {
  std::vector<gameinput::DeviceID> input_ids;
  fill_input_ids(input_ids, first, rest...);
  auto p = Profile(input_ids);
  auto devices = get_devices(&p, first, rest...);
  return std::tuple_cat(std::make_tuple(std::move(p)), devices);
}

namespace vjoyids {
  const VJOY_ID
		VJOY_1 { 1 }, VJOY_2 { 2 }, VJOY_3 { 3 }, VJOY_4 { 4 },
		VJOY_5 { 5 }, VJOY_6 { 6 }, VJOY_7 { 7 }, VJOY_8 { 8 },
		VJOY_9 { 9 }, VJOY_10 { 10 }, VJOY_11 { 11 }, VJOY_12 { 12 },
		VJOY_13 { 13 }, VJOY_14 { 14 }, VJOY_15 { 15 }, VJOY_16 { 16 };
}

} // namespace fredemmott::inputmapping
