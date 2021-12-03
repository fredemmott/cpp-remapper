/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "Profile.h"

#include <algorithm>
#include <cstdio>

#include "HidHide.h"
#include "InputDevice.h"
#include "InputDeviceCollection.h"
#include "Mapper.h"
#include "VJoyDevice.h"

using namespace fredemmott::inputmapping;

namespace fredemmott::inputmapping {
struct Profile::Impl {
  std::unique_ptr<Mapper> mapper;
  std::unique_ptr<HidHide> guardian;
};

Profile::Profile(const std::vector<DeviceSpecifier>& ids)
  : p(new Impl {std::make_unique<Mapper>(), std::make_unique<HidHide>(ids)}) {
}

Profile::Profile(Profile&& moved) : p(std::move(moved.p)) {
}

Profile::~Profile() {
}

Mapper* Profile::operator->() const {
  return p->mapper.get();
}

}// namespace fredemmott::inputmapping

namespace fredemmott::inputmapping::detail {

MappableInput get_device(InputDeviceCollection* c, const DeviceSpecifier& id) {
  auto device = c->get(id);
  if (!device) {
    auto desc = id.getHumanReadable();
    printf("ERROR: Failed to find device '%s'\n", desc.c_str());
    exit(0);
  }
  MappableInput ret(device);
  auto name = device->getProductName();
  auto instance_id = device->getInstanceID().getHumanReadable();
  auto hardware_id = device->getHardwareID().getHumanReadable();
  printf(
    "Found \"%s\":\n  %s\n  %s\n",
    name.c_str(),
    instance_id.c_str(),
    hardware_id.c_str());
  return ret;
}

std::tuple<> get_devices(Profile*, InputDeviceCollection*) {
  return std::make_tuple();
}

std::vector<MappableInput> select_inputs() {
  return {};
}

std::vector<std::shared_ptr<OutputDevice>> select_outputs() {
  return {};
}

void fill_input_ids(std::vector<DeviceSpecifier>&) {
}
}// namespace fredemmott::inputmapping::detail

#include "connections.h"
#include "devicedb.h"

namespace fredemmott::inputmapping {

namespace {

using namespace fredemmott::inputmapping::devicedb;

void static_test_vjoy() {
  auto [p, stick, vj1] = create_profile(VPC_RIGHT_WARBRD, VJOY_1);
  stick.Button1 >> vj1.Button1;
}

void static_test_vigem() {
  auto [p, stick, xpad, ds4]
    = create_profile(VPC_RIGHT_WARBRD, VIGEM_X360_PAD, VIGEM_DS4_PAD);
  stick.Button1 >> xpad.ButtonA;
  stick.Button2 >> ds4.ButtonTriangle;
}

}// namespace

}// namespace fredemmott::inputmapping
