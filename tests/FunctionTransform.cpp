/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "FunctionTransform.h"
#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("FunctionTransform") {
  Axis::Value value = -1;

  TestAxis axis;
  axis >> [](Axis::Value value) { return Axis::MAX - value; } >> &value;
  axis.emit(Axis::MAX);
  REQUIRE(value == 0);
}
