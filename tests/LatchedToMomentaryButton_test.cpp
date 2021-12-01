/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "LatchedToMomentaryButton.h"

#include "FakeClock.h"
#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("LatchedToMomentaryButton") {
  auto clock = std::make_shared<FakeClock>();
  Clock::set(clock);

  TestButton button;
  Button::Value pressed(false);

  button >> LatchedToMomentaryButton() >> &pressed;
  button.emit(true);
  REQUIRE(pressed);
  clock->advance(std::chrono::milliseconds(1));
  REQUIRE(pressed);
  clock->advance(std::chrono::seconds(1));
  REQUIRE(!pressed);

  button.emit(false);
  REQUIRE(pressed);
  clock->advance(std::chrono::seconds(1));
  REQUIRE(!pressed);
}
