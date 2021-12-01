/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "ButtonToAxis.h"

#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("ButtonToAxis") {
  TestButton button;
  Axis::Value out = -1;
  SECTION("Defaults") {
    button >> ButtonToAxis() >> &out;
    button.emit(false);
    REQUIRE(out == Axis::MIN);
    button.emit(true);
    REQUIRE(out == Axis::MAX);
  }

  SECTION("Custom values") {
    button >> ButtonToAxis(42, 1337) >> &out;
    button.emit(false);
    REQUIRE(out == 42);
    button.emit(true);
    REQUIRE(out == 1337);
  }
}