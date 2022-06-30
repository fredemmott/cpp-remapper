/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "HatToButtons.h"

#include "MappableVJoyOutput.h"
#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("HatToButtons") {
  TestHat hat;
  Button::Value c, n, e, s, w;

  SECTION("Without Center") {
    hat >> HatToButtons(&n, &e, &s, &w);
    hat.emit(Hat::CENTER);
    REQUIRE(!n);
    REQUIRE(!e);
    REQUIRE(!s);
    REQUIRE(!w);

    hat.emit(Hat::NORTH);
    REQUIRE(n);
    REQUIRE(!e);
    REQUIRE(!s);
    REQUIRE(!w);

    hat.emit(Hat::NORTH_EAST);
    REQUIRE(n);
    REQUIRE(e);
    REQUIRE(!s);
    REQUIRE(!w);

    hat.emit(Hat::EAST);
    REQUIRE(!n);
    REQUIRE(e);
    REQUIRE(!s);
    REQUIRE(!w);

    hat.emit(Hat::SOUTH_EAST);
    REQUIRE(!n);
    REQUIRE(e);
    REQUIRE(s);
    REQUIRE(!w);

    hat.emit(Hat::SOUTH);
    REQUIRE(!n);
    REQUIRE(!e);
    REQUIRE(s);
    REQUIRE(!w);

    hat.emit(Hat::SOUTH_WEST);
    REQUIRE(!n);
    REQUIRE(!e);
    REQUIRE(s);
    REQUIRE(w);

    hat.emit(Hat::WEST);
    REQUIRE(!n);
    REQUIRE(!e);
    REQUIRE(!s);
    REQUIRE(w);

    hat.emit(Hat::NORTH_WEST);
    REQUIRE(n);
    REQUIRE(!e);
    REQUIRE(!s);
    REQUIRE(w);

    hat.emit(Hat::CENTER);
    REQUIRE(!n);
    REQUIRE(!e);
    REQUIRE(!s);
    REQUIRE(!w);

    // Let's spot-check a few edge cases: have 8 different button
    // combinations, so the boundaries should be at a 16th of a circle
    const auto delta = std::lround(Hat::MAX / 16.0);

    hat.emit(Hat::NORTH + delta);
    REQUIRE(n);
    REQUIRE(!e);

    hat.emit(Hat::NORTH + delta + 1);
    REQUIRE(n);
    REQUIRE(e);

    hat.emit(Hat::SOUTH + delta);
    REQUIRE(s);
    REQUIRE(!w);

    hat.emit(Hat::SOUTH + delta + 1);
    REQUIRE(s);
    REQUIRE(w);

    hat.emit(Hat::NORTH_WEST + delta);
    REQUIRE(n);
    REQUIRE(w);

    hat.emit(Hat::NORTH_WEST + delta + 1);
    REQUIRE(n);
    REQUIRE(!w);
  }

  SECTION("With Center") {
    hat >> HatToButtons(HatToButtons::CenterButton(&c), &n, &e, &s, &w);
    hat.emit(Hat::CENTER);
    REQUIRE(c);
    REQUIRE(!n);
    REQUIRE(!e);
    REQUIRE(!s);
    REQUIRE(!w);

    hat.emit(Hat::NORTH);
    REQUIRE(!c);
    REQUIRE(n);

    hat.emit(Hat::EAST);
    REQUIRE(!c);
    REQUIRE(!n);
    REQUIRE(e);

    hat.emit(Hat::CENTER);
    REQUIRE(c);
    REQUIRE(!n);
    REQUIRE(!e);
    REQUIRE(!s);
    REQUIRE(!w);
  }

  SECTION("Without Interpolation") {
    hat >> HatToButtons(HatToButtons::Interpolation::None, &n, &e, &s, &w);
    hat.emit(Hat::NORTH);
    REQUIRE(n);
    REQUIRE(!e);
    REQUIRE(!s);
    REQUIRE(!w);
    hat.emit(Hat::NORTH + 1);
    REQUIRE(!n);
    REQUIRE(!e);
    hat.emit(Hat::MAX - 1);
    REQUIRE(!n);
    REQUIRE(!w);
    hat.emit(Hat::EAST);
    REQUIRE(!n);
    REQUIRE(!w);
    REQUIRE(!s);
    hat.emit(Hat::EAST - 1);
    REQUIRE(!n);
    REQUIRE(!e);
    hat.emit(Hat::EAST + 1);
    REQUIRE(!e);
    REQUIRE(!s);
  }
}

namespace {
static void static_test() {
  MappableVJoyOutput o(nullptr);
  HatToButtons(&o, 123, 4);
  bool center;
  HatToButtons(HatToButtons::CenterButton(&center), &o, 123, 4);
}
}// namespace
