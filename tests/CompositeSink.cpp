/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "CompositeSink.h"

#include <concepts>

#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("CompositeSink") {
  TestAxis axis;

  Axis::Value out1(-1), out2(-1), out3(-1);
  // 1, 2, n are special
  axis >> all([&](Axis::Value v) { out1 = v; });
  axis.emit(Axis::MAX);
  REQUIRE(out1 == Axis::MAX);
  axis
    >> all([&](Axis::Value v) { out1 = v; }, [&](Axis::Value v) { out2 = v; });
  axis.emit(Axis::MIN);
  REQUIRE(out1 == Axis::MIN);
  REQUIRE(out2 == Axis::MIN);
  axis >> all(
    [&](Axis::Value v) { out1 = v; },
    [&](Axis::Value v) { out2 = v; },
    [&](Axis::Value v) { out3 = v; });
  axis.emit(Axis::MAX);
  REQUIRE(out1 == Axis::MAX);
  REQUIRE(out2 == Axis::MAX);
  REQUIRE(out3 == Axis::MAX);
}
