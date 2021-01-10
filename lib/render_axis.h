/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"
#include "eventhandler.h"

#include <functional>

namespace fredemmott::inputmapping {

void render_axis(
  const std::string& bmp_filename,
  std::function<AxisEventHandler(const AxisEventHandler&)> factory
);

} // namespace fredemmott::inputmapping
