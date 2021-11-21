/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <functional>
#include <type_traits>

#include "TransformRef.h"
#include "actionsapi.h"
#include "FunctionTransform.h"

namespace fredemmott::inputmapping {

void render_axis(
  const std::string& bmp_filename,
  AxisSinkRef transform_in,
  AxisSourceRef transform_out);

template<transform<Axis, Axis> T>
void render_axis(const std::string& bmp_filename, const T& transform) {
  render_axis(
    bmp_filename, AxisOutputRef(&transform), AxisInputRef(&transform));
}

template <transform_invocable<Axis, Axis> T>
void render_axis(const std::string& bmp_filename, const T& func) {
  FunctionTransform transform(func);
  render_axis(
    bmp_filename, AxisOutputRef(&transform), AxisInputRef(&transform));
}

}// namespace fredemmott::inputmapping
