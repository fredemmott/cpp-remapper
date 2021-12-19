/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "AxisTrimmer.h"

#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("SimpleRecenter") {
  AxisTrimmer trim;
  Axis::Value out;
  trim >> &out;
  trim.map(Axis::MID + 100);
  REQUIRE(out == Axis::MID + 100);
  trim.TrimButton->map(true);
  trim.map(Axis::MID);
  REQUIRE(out == Axis::MID + 100);
  trim.TrimButton->map(false);
  REQUIRE(out == Axis::MID + 100);
  trim.map(Axis::MID + 1);
  REQUIRE(out == Axis::MID + 101);
}

TEST_CASE("OffCenterTrim") {
  AxisTrimmer trim;
  Axis::Value out;
  trim >> &out;

  static_assert(Axis::MID > 100);
  static_assert(Axis::MAX > 200);

  trim.map(200);
  trim.TrimButton->map(true);
  trim.map(100);
  trim.TrimButton->map(false);
  REQUIRE(out == 200);
  trim.map(200);
  REQUIRE(out == 300);
}

TEST_CASE("ResetTrim") {
  AxisTrimmer trim;
  Axis::Value out;
  trim >> &out;
  trim.map(Axis::MID + 100);
  trim.TrimButton->map(true);
  trim.map(Axis::MID);
  trim.TrimButton->map(false);
  REQUIRE(out == Axis::MID + 100);
  trim.ResetButton->map(true);
  trim.ResetButton->map(false);
  REQUIRE(out == Axis::MID);
}

TEST_CASE("TrimmerBounds") {
  AxisTrimmer trim;
  Axis::Value out;
  trim >> &out;

  SECTION("Trim to MAX") {
    trim.map(Axis::MAX);
    REQUIRE(out == Axis::MAX);
    trim.TrimButton->map(true);
    trim.map(Axis::MID);
    trim.TrimButton->map(false);
    REQUIRE(out == Axis::MAX);
    trim.map(Axis::MID + 1);
    REQUIRE(out == Axis::MAX);
    trim.map(Axis::MAX);
    REQUIRE(out == Axis::MAX);
    trim.map(Axis::MID - 1);
    REQUIRE(out == Axis::MAX - 1);
    trim.map(Axis::MIN);
    REQUIRE(out == Axis::MAX - Axis::MID);
  }

  SECTION("Trim to MIN") {
    trim.ResetButton->map(true);
    trim.ResetButton->map(false);
    trim.map(Axis::MIN);
    trim.TrimButton->map(true);
    trim.map(Axis::MID);
    trim.TrimButton->map(false);
    REQUIRE(out == Axis::MIN);
    trim.map(Axis::MID - 1);
    REQUIRE(out == Axis::MIN);
    trim.map(Axis::MIN);
    REQUIRE(out == Axis::MIN);
    trim.map(Axis::MAX);
    REQUIRE(out == Axis::MAX - Axis::MID);
  }
}
