/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include <cpp-remapper/Shift.h>

#include <cpp-remapper/MappableVJoyOutput.h>
#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("Shift") {
  bool shifted = false;
  TestButton button;
  bool a = false, b = false;

  button >> Shift(&shifted, &a, &b);

  button.emit(true);
  REQUIRE(a);
  REQUIRE(!b);

  button.emit(false);
  REQUIRE(!a);
  REQUIRE(!b);

  shifted = true;
  button.emit(true);
  REQUIRE(!a);
  REQUIRE(b);
}
