/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include <cpp-remapper/FunctionSink.h>

#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("FunctionSink") {
  Axis::Value value = -1;

  TestAxis axis;
  axis >> [&value](Axis::Value newValue) { value = newValue; };
  axis.emit(1337);
  REQUIRE(value == 1337);
}
