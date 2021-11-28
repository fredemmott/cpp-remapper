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

  // For now, relying on static_test below
#ifdef TODO
  Axis::Value out1(-1), out2(-1), out3(-1);
  // 1, 2, n are special
  axis >> &out1;
  axis.emit(Axis::MAX);
  REQUIRE(out1 == Axis::MAX);
  axis >> ::fredemmott::inputmapping::all(&out1, &out2);
  axis.emit(Axis::MIN);
  REQUIRE(out1 == Axis::MIN);
  REQUIRE(out2 == Axis::MIN);
  axis >> ::fredemmott::inputmapping::all(&out1, &out2, &out3);
  axis.emit(Axis::MAX);
  REQUIRE(out1 == Axis::MAX);
  REQUIRE(out2 == Axis::MAX);
  REQUIRE(out3 == Axis::MAX);
#endif
}

namespace {

void static_test() {
  ButtonSinkRef b = [](bool) {};
  auto single_button = all(b);
  auto two_buttons = all(b, b);
  auto many_buttons = all(b, b, b, b, b);

  auto lambda = all([](bool) {});
  auto many_lambdas = all([](bool) {}, [](bool) {}, [](bool) {});
  static_assert(std::same_as<CompositeSink<Button>, decltype(many_lambdas)>);

  auto many_axis_lambdas = all([](long) {}, [](long) {}, [](long) {});
  static_assert(!std::same_as<CompositeSink<Axis>, decltype(many_lambdas)>);
  static_assert(std::same_as<CompositeSink<Axis>, decltype(many_axis_lambdas)>);
}

}// namespace
