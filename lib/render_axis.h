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

#include "actionsapi.h"

namespace fredemmott::inputmapping {

void render_axis(
  const std::string& bmp_filename,
  AxisOutput& transform_in,
  AxisInput& transform_out);

template <typename T>
void render_axis(const std::string& bmp_filename, T& transform) {
  render_axis(bmp_filename, AxisOutput(&transform), AxisInput(&transform));
}

template <
  typename T,
  std::enable_if_t<
    std::is_convertible_v<T, std::function<Axis::Value(Axis::Value)>>,
    bool> = true>
void render_axis(const std::string& bmp_filename, const T& func) {
  FunctionTransform<AxisSink, AxisSource> transform(func);
  render_axis(bmp_filename, AxisOutput(&transform), AxisInput(&transform));
}

}// namespace fredemmott::inputmapping
