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

Profile::Profile(const std::vector<HiddenDevice>& ids)
  : p(new Impl {
    std::make_unique<Mapper>(),
    std::make_unique<HidHide>(
      std::vector<DeviceSpecifier>(ids.begin(), ids.end()))}) {
}

Profile::Profile(Profile&& moved) : p(std::move(moved.p)) {
}

Profile::~Profile() {
}

Mapper* Profile::operator->() const {
  return p->mapper.get();
}

DeviceWithVisibility::DeviceWithVisibility(const DeviceSpecifier& ds)
  : impl(ds) {
}

DeviceWithVisibility::operator const DeviceSpecifier&() const {
  return impl;
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

std::vector<std::shared_ptr<EventSource>> get_event_sources() {
  return {};
}

std::vector<std::shared_ptr<EventSink>> get_event_sinks() {
  return {};
}

void fill_hidden_ids(std::vector<HiddenDevice>&) {
}

}// namespace fredemmott::inputmapping::detail
