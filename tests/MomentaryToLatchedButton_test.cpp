/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include <cpp-remapper/MomentaryToLatchedButton.h>

#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("MomentaryToLatchedButton") {

  TestButton button;
  Button::Value pressed(false);

  button >> MomentaryToLatchedButton() >> &pressed;
  button.emit(true);
  REQUIRE(pressed);
  button.emit(false);
  REQUIRE(pressed);

  button.emit(true);
  REQUIRE(!pressed);
  button.emit(false);
  REQUIRE(!pressed);

  button.emit(true);
  REQUIRE(pressed);
  button.emit(false);
  REQUIRE(pressed);
}
