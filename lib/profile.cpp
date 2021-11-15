/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "profile.h"

#include "hidhide.h"
#include "inputdevice.h"
#include "inputdevicecollection.h"
#include "mapper.h"
#include "VJoyDevice.h"

#include <algorithm>
#include <cstdio>

using namespace fredemmott::gameinput;

namespace fredemmott::inputmapping {
  struct Profile::Impl {
    std::vector<gameinput::DeviceSpecifier> ids;
    std::unique_ptr<Mapper> mapper;
    std::unique_ptr<HidHide> guardian;
    std::unique_ptr<InputDeviceCollection> deviceCollection;
  };

  Profile::Profile(const std::vector<gameinput::DeviceSpecifier>& ids):
    p(new Impl {
      ids,
      std::make_unique<Mapper>(),
      std::make_unique<HidHide>(ids),
      std::make_unique<InputDeviceCollection>(),
    }) {
    std::reverse(p->ids.begin(), p->ids.end());
  }

  Profile::Profile(Profile&& moved): p(std::move(moved.p)) {
  }

  Profile::~Profile() {
  }

  MappableInput Profile::popInput() {
    const auto id = p->ids.back();
    p->ids.pop_back();
    MappableInput ret(p->deviceCollection->get(id));
    auto device = ret.getDevice();
    auto name = device->getProductName();
    auto instance_id = device->getInstanceID().getHumanReadable();
    auto hardware_id = device->getHardwareID().getHumanReadable();
    printf("Found \"%s\":\n  %s\n  %s\n", name.c_str(), instance_id.c_str(), hardware_id.c_str());
    return ret;
  }

  Mapper* Profile::operator-> () const {
    return p->mapper.get();
  }
} // namespace fredemmott::inputmapping
