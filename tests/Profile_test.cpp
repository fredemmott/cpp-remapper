/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "Profile.h"

#include "OutputDevice.h"
#include "connections.h"
#include "devicedb.h"
#include "tests.h"

using namespace fredemmott::inputmapping;
using namespace fredemmott::inputmapping::devicedb;

static void static_test_vjoy() {
  auto [p, stick, vj1] = create_profile(VPC_RIGHT_WARBRD, VJOY_1);
  stick.Button1 >> vj1.Button1;
}

static void static_test_vigem() {
  auto [p, stick, xpad, ds4]
    = create_profile(VPC_RIGHT_WARBRD, VIGEM_X360_PAD, VIGEM_DS4_PAD);
  // Device-specific IDs
  stick.Button1 >> xpad.ButtonA;
  stick.Button2 >> ds4.ButtonTriangle;
}

static void static_test_unhiddendevice() {
  auto [p, stick, vj1]
    = create_profile(UnhiddenDevice(VPC_RIGHT_WARBRD), VJOY_1);
  stick.Button1 >> vj1.Button1;
}

TEST_CASE("TestFillHiddenIDs") {
  static_assert(std::convertible_to<decltype(VIDPID(0, 0)), DeviceSpecifier>);
  static_assert(!std::convertible_to<decltype(VIDPID(0, 0)), HiddenDevice>);
  static_assert(!std::convertible_to<decltype(VIDPID(0, 0)), UnhiddenDevice>);
  static_assert(std::is_constructible_v<HiddenDevice, VIDPID>);
  static_assert(!std::is_constructible_v<HiddenDevice, UnhiddenDevice>);

  std::vector<HiddenDevice> hidden;
  detail::fill_hidden_ids(
    hidden,
    VIDPID {0x1234, 0x0001},
    HiddenDevice(VIDPID {0x1234, 0x0002}),
    VJOY_1,
    VJOY_2,
    VIGEM_X360_PAD,
    VIGEM_DS4_PAD);
  REQUIRE(hidden.size() == 2);

  hidden.clear();
  detail::fill_hidden_ids(
    hidden,
    VIDPID {0x1234, 0x0001},
    UnhiddenDevice(VIDPID {0x1234, 0x0002}),
    HiddenDevice(VIDPID {0x1234, 0x0003}),
    VJOY_1,
    VJOY_2,
    VIGEM_X360_PAD,
    VIGEM_DS4_PAD);
  REQUIRE(hidden.size() == 2);
}
