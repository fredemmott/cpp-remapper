/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include <cpp-remapper/CompositeSink.h>

#include <cpp-remapper/MappableVJoyOutput.h>
#include <cpp-remapper/SquareDeadzone.h>
#include <cpp-remapper/connections.h>
#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("CompositeSink") {
  TestAxis axis;

  SECTION("Lambdas") {
    Axis::Value out1(-1), out2(-1), out3(-1);
    // 1, 2, n are special
    axis >> all([&](Axis::Value v) { out1 = v; });
    axis.emit(Axis::MAX);
    REQUIRE(out1 == Axis::MAX);
    axis >> all(
      [&](Axis::Value v) { out1 = v; }, [&](Axis::Value v) { out2 = v; });
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

  SECTION("Pipelines") {
    Axis::Value out1(-1), out2(-1);
    axis >> all(
      SquareDeadzone(10_percent) >> &out1, SquareDeadzone(20_percent) >> &out2);
    axis.emit(Axis::MID + 1);
    REQUIRE(out1 == Axis::MID);
    REQUIRE(out2 == Axis::MID);
    axis.emit(Axis::MID + (Axis::MAX / 10) + 1);
    REQUIRE(out1 > Axis::MID);
    REQUIRE(out2 == Axis::MID);
    axis.emit(Axis::MID + (Axis::MAX / 5) + 1);
    REQUIRE(out1 > Axis::MID);
    REQUIRE(out2 > Axis::MID);
  }
}

namespace {
void static_test_ptrs() {
  MappableVJoyOutput vj(nullptr);
  TestAxis axis;
  static_assert(any_sink_ptr<decltype(vj.XAxis)>);
  static_assert(any_sink_ptr<decltype(vj.YAxis)>);
  static_assert(any_sink_ptr<decltype(vj.ZAxis)>);
  axis >> all(vj.XAxis, vj.YAxis, vj.ZAxis);
}
}// namespace
