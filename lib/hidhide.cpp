/*
 *r Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "hidhide.h"

#include "inputdevice.h"
#include "inputdevicecollection.h"

#include <iomanip>
#include <sstream>
#include <cstdio>

#include "HidHideCLI/stdafx.h"
#include "HidHideCLI/FilterDriverProxy.h"

using HidHideHandle = HidHide::FilterDriverProxy;

namespace fredemmott::gameinput
{
  HidHide::HidHide(const std::vector<DeviceSpecifier> &specifiers)
  {
    printf("Configuring HidHide...\n");
    InputDeviceCollection collection;
    ::HidHide::FilterDriverProxy hidhide(/* autosave = */ false);
    auto existing = hidhide.GetBlacklist();
    for (const auto &specifier: specifiers)
    {
      auto device = collection.get(specifier);
      if (!device) {
        continue;
      }
      auto instance = device->getInstanceID();
      mInstances.push_back(instance);
      hidhide.BlacklistAddEntry(instance.toWString());
    }
    hidhide.SetActive(true);
    hidhide.ApplyConfigurationChanges();
  }

  HidHide::~HidHide()
  {
    printf("Cleaning up HidHide configuration...\n");
    ::HidHide::FilterDriverProxy hidhide(/*autosave = */ false);
    for (const auto& instance: mInstances) {
      hidhide.BlacklistDelEntry(instance.toWString());
    }
    hidhide.ApplyConfigurationChanges();
  }

} // fredemmott::gameinput
