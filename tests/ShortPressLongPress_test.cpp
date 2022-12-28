/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include <cpp-remapper/ShortPressLongPress.h>

#include "FakeClock.h"
#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("ShortPressLongPress") {
  auto clock = std::make_shared<FakeClock>();
  Clock::set(clock);

  TestButton button;
  Button::Value b1(false), b2(false);

  button >> ShortPressLongPress(&b1, &b2);
  SECTION("Short Press") {
    button.emit(true);
    clock->advance(std::chrono::milliseconds(1));
    REQUIRE(!b1);
    REQUIRE(!b2);
    button.emit(false);
    REQUIRE(b1);
    REQUIRE(!b2);
    clock->advance(std::chrono::seconds(1));
    REQUIRE(!b1);
  }

  SECTION("Long Press") {
    button.emit(true);
    clock->advance(std::chrono::seconds(10));
    button.emit(false);
    REQUIRE(!b1);
    REQUIRE(b2);
    clock->advance(std::chrono::seconds(1));
    REQUIRE(!b1);
    REQUIRE(!b2);
  }
}
