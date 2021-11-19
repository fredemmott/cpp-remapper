/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <functional>

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

}// namespace fredemmott::inputmapping
