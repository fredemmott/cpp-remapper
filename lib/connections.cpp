/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "connections.h"

#include <concepts>

#include "axiscurve.h"
#include "MappableInput.h"
#include "MappableVJoyOutput.h"
#include "squaredeadzone.h"

namespace fredemmott::inputmapping {
namespace {

void static_test_transform_pipelines() {
  auto x
    = std::make_shared<SquareDeadzone>(50) >> std::make_shared<AxisCurve>(0.5);
  static_assert(std::same_as<decltype(x), TransformPipeline<Axis, Axis>>);

  auto y
    = std::make_shared<SquareDeadzone>(50) >> AxisCurve(0.5);
  static_assert(std::same_as<decltype(x), decltype(y)>);

  auto z = SquareDeadzone(50) >> AxisCurve(0.5);
  static_assert(std::same_as<decltype(x), decltype(z)>);
}

void static_test_transfomed_source_pipelines() {
  MappableInput stick(nullptr);
  MappableVJoyOutput vj1(0);
  stick.XAxis >> SquareDeadzone(10) >> AxisCurve(-0.5) >> vj1.XAxis;
}

}// namespace
}// namespace fredemmott::inputmapping
