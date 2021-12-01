/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "AxisToButtons.h"

#include "CompositeSink.h"
#include "MappableVJoyOutput.h"
#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("AxisToButtons") {
  TestAxis axis;
  SECTION("Implicit Equal Spacing") {
    Button::Value b1, b2, b3;
    axis >> AxisToButtons(&b1, &b2, &b3);

    axis.emit(0);
    REQUIRE(b1);
    REQUIRE(!b2);
    REQUIRE(!b3);
    axis.emit(Axis::MAX / 3);
    REQUIRE(b1);
    REQUIRE(!b2);
    REQUIRE(!b3);
    axis.emit((Axis::MAX / 3) + 1);
    REQUIRE(!b1);
    REQUIRE(b2);
    REQUIRE(!b3);
    axis.emit(2 * Axis::MAX / 3);
    REQUIRE(!b1);
    REQUIRE(b2);
    REQUIRE(!b3);
    axis.emit((2 * Axis::MAX / 3) + 1);
    REQUIRE(!b1);
    REQUIRE(!b2);
    REQUIRE(b3);
    axis.emit(Axis::MAX);
    REQUIRE(!b1);
    REQUIRE(!b2);
    REQUIRE(b3);
  }

  SECTION("Explicit ranges") {
    Button::Value b1, b2;
    axis >> AxisToButtons {{0, 0, &b1}, {100, 100, &b2}};
    axis.emit(0);
    REQUIRE(b1);
    REQUIRE(!b2);
    axis.emit(1);
    REQUIRE(!b1);
    REQUIRE(!b2);
    axis.emit(Axis::MAX - 1);
    REQUIRE(!b1);
    REQUIRE(!b2);
    axis.emit(Axis::MAX);
    REQUIRE(!b1);
    REQUIRE(b2);
  }
}

namespace {

// Check that the compiler lets us call it as intended
void static_test_real_buttons() {
  MappableVJoyOutput o(nullptr);
  AxisToButtons::Range range1 {0, 0, o.Button1};

  AxisToButtons::Range range2 {0, 0, all(o.Button1, o.Button2)};
  AxisToButtons foo {
    {0, 0, o.Button1}, {0, 0, all(o.Button1, o.Button2)}, {100, 100, [](bool) {
                                                           }}};
  AxisToButtons implicit_equal_spacing(o.Button1, o.Button2, o.Button3);
}

}// namespace
