/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "AnyOfButton.h"

#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("AnyOfButton") {
  AnyOfButton foo;
  bool pressed = false;

  foo >> &pressed;

  foo.map(true); // 0 -> 1
  REQUIRE(pressed == true);
  foo.map(true); // 1 -> 2
  REQUIRE(pressed == true);
  foo.map(false); // 2 -> 1
  REQUIRE(pressed == true);
  foo.map(false); // 1 -> 0
  REQUIRE(pressed == false);
}
