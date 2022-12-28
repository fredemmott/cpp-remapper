/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <functional>
#include <string>
#include <type_traits>

#include "FunctionTransform.h"
#include "SinkPtr.h"
#include "SourcePtr.h"
#include "TransformPtr.h"

namespace fredemmott::inputmapping {

void render_axis(
  const std::string& bmp_filename,
  AxisSinkPtr transform_in,
  AxisSourcePtr transform_out);

template <transform_ptr<Axis, Axis> T>
void render_axis(const std::string& bmp_filename, const T& transform) {
  render_axis(bmp_filename, transform, transform);
}

template <transform<Axis, Axis> T>
void render_axis(const std::string& bmp_filename, T&& transform) {
  render_axis(bmp_filename, std::move(transform));
}

template <transform_invocable<Axis, Axis> T>
void render_axis(const std::string& bmp_filename, const T& func) {
  auto t = std::make_shared<FunctionTransform<Axis, Axis>>(func);
  render_axis(bmp_filename, t);
}

}// namespace fredemmott::inputmapping
