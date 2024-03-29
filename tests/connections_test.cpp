/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include <cpp-remapper/connections.h>

#include <concepts>

#include <cpp-remapper/AxisCurve.h>
#include <cpp-remapper/AxisToButtons.h>
#include <cpp-remapper/MappableInput.h>
#include <cpp-remapper/MappableVJoyOutput.h>
#include <cpp-remapper/SquareDeadzone.h>
#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("SourcePtr >> Value*") {
  Axis::Value out(-1);
  auto axis = std::make_shared<TestAxis>();
  axis >> &out;
  axis->emit(123);
  REQUIRE(out == 123);
}

TEST_CASE("Source& >> Value*") {
  Axis::Value out = -1;
  TestAxis axis;
  axis >> &out;
  axis.emit(123);
  REQUIRE(out == 123);
}

TEST_CASE("Source& >> Sink&&") {
  bool b1 = false, b2 = false;
  TestAxis axis;
  axis >> AxisToButtons {
    {0_percent, 10_percent, [&b1](bool value) { b1 = value; }},
    {90_percent, 100_percent, [&b2](bool value) { b2 = value; }}};
  axis.emit(0);
  REQUIRE(b1);
  REQUIRE(!b2);
  axis.emit(Axis::MAX / 10);
  REQUIRE(b1);
  REQUIRE(!b2);
  axis.emit(Axis::MAX / 2);
  REQUIRE(!b1);
  REQUIRE(!b2);
  axis.emit(Axis::MAX);
  REQUIRE(!b1);
  REQUIRE(b2);
}

TEST_CASE("Source& >> Transform*") {
  // Testing the plumbing of >>, not the specific transform
  long out = -1;
  TestAxis axis;
  SquareDeadzone dz(10_percent);

  axis >> &dz >> &out;
  axis.emit(0);
  REQUIRE(out == 0);
  axis.emit(Axis::MAX);
  REQUIRE(out == Axis::MAX);
  axis.emit(100);
  REQUIRE(out > 100);
  axis.emit(Axis::MAX - 100);
  REQUIRE(out < Axis::MAX - 100);
}

TEST_CASE("Source& >> Transform&&") {
  // Testing the plumbing of >>, not the specific transform
  long out = -1;
  TestAxis axis;
  SquareDeadzone dz(10_percent);

  // Via temporary
  axis >> SquareDeadzone {10_percent} >> &out;
  axis.emit(0);
  REQUIRE(out == 0);
  axis.emit(Axis::MAX);
  REQUIRE(out == Axis::MAX);
}

namespace {

void static_test_transform_pipelines() {
  auto x = std::make_shared<SquareDeadzone>(50_percent)
    >> std::make_shared<AxisCurve>(0.5);
  static_assert(
    std::same_as<decltype(x), std::shared_ptr<TransformPipeline<Axis, Axis>>>);

  auto y = std::make_shared<SquareDeadzone>(50_percent) >> AxisCurve(0.5);
  static_assert(std::same_as<decltype(x), decltype(y)>);

  auto z = SquareDeadzone(50_percent) >> AxisCurve(0.5);
  static_assert(std::same_as<decltype(x), decltype(z)>);
}

void static_test_transfomed_source_pipelines() {
  MappableInput stick(nullptr);
  MappableVJoyOutput vj(nullptr);
  stick.XAxis >> SquareDeadzone(10_percent) >> AxisCurve(-0.5) >> vj.XAxis;
}

}// namespace
