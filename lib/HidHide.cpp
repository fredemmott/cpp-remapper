/*
 *r Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "HidHide.h"

#include <cstdio>
#include <iomanip>
#include <sstream>

// clang-format off
#include "HidHideCLI/stdafx.h"
#include "HidHideCLI/FilterDriverProxy.h"
// clang-format on
#include "InputDevice.h"
#include "InputDeviceCollection.h"

namespace fredemmott::inputmapping {
HidHide::HidHide(const std::vector<DeviceSpecifier>& specifiers) {
  printf("Configuring HidHide...\n");
  try {
    init(specifiers);
    mInitialized = true;
  } catch (const std::runtime_error& e) {
    fprintf(
      stderr,
      "---\n"
      "!!! WARNING !!!\n"
      "HidHide failed to initialize: %s\n"
      "\n"
      "Close any other programs using HidHide (including the configuration "
      "utility) and try again.\n"
      "---\n",
      e.what());
  }
}

void HidHide::init(const std::vector<DeviceSpecifier>& specifiers) {
  InputDeviceCollection collection;
  ::HidHide::FilterDriverProxy hidhide(/* autosave = */ false);
  auto existing = hidhide.GetBlacklist();
  for (const auto& specifier: specifiers) {
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

HidHide::~HidHide() {
  if (!mInitialized) {
    printf("Skipping HidHide cleanup - failed to initialize on startup.\n");
    return;
  }

  try {
    printf("Cleaning up HidHide configuration...\n");
    deinit();
    printf("...cleaned up HidHide.\n");
  } catch (const std::runtime_error& e) {
    fprintf(
      stderr,
      "---\n"
      "!!! WARNING !!!\n"
      "Failed to clean up HidHide: %s\n"
      "\n"
      "Close any other programs using HidHide (including the configuration "
      "utility) and try again.\n"
      "---\n",
      e.what());
  }
}

void HidHide::deinit() {
  ::HidHide::FilterDriverProxy hidhide(/*autosave = */ false);
  for (const auto& instance: mInstances) {
    hidhide.BlacklistDelEntry(instance.toWString());
  }
  hidhide.ApplyConfigurationChanges();
}

}// namespace fredemmott::inputmapping
