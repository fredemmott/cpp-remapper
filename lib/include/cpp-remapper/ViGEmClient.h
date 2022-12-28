/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

// clang-format off
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ViGEm/Client.h>
// clang-format on

namespace fredemmott::inputmapping {

class ViGEmClient final {
 public:
  ViGEmClient();
  ~ViGEmClient();

  operator PVIGEM_CLIENT();
  operator bool();

  static ViGEmClient& get();

  ViGEmClient(const ViGEmClient&) = delete;
  ViGEmClient& operator=(const ViGEmClient&) = delete;

 private:
  PVIGEM_CLIENT p = nullptr;
};

}// namespace fredemmott::inputmapping