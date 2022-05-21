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

#include "EventLoop.h"
#include "HidHide.h"
#include "InputDevice.h"
#include "InputDeviceCollection.h"
#include "MappableInput.h"
#include "MappableVJoyOutput.h"
#include "VJoyDevice.h"
#include "connections.h"

using namespace fredemmott::inputmapping;

namespace fredemmott::inputmapping {
struct Profile::Impl {
  std::shared_ptr<EventLoop> EventLoop;
  std::unique_ptr<HidHide> guardian;
};

Profile::Profile(const std::vector<HiddenDevice>& ids) : p(std::make_unique<Impl>()) {
  std::vector<DeviceSpecifier> specifiers;
  std::ranges::transform(ids, std::back_inserter(specifiers), [](auto it) {
    return it.getSpecifier();
  });
  *p = {
    std::make_shared<EventLoop>(),
    std::make_unique<HidHide>(specifiers),
  };
}

Profile::Profile(Profile&& moved) : p(std::move(moved.p)) {
}

Profile::~Profile() {
}

std::shared_ptr<EventLoop> Profile::getEventLoop() const {
  return p->EventLoop;
}

void Profile::run() {
  getEventLoop()->run();
}

DeviceWithVisibility::DeviceWithVisibility(const DeviceSpecifier& ds)
  : impl(ds) {
}

DeviceSpecifier DeviceWithVisibility::getSpecifier() const {
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
