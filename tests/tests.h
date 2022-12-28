/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <catch.hpp>

#include <cpp-remapper/Source.h>
#include <cpp-remapper/connections.h>

namespace fredemmott::inputmapping {

template <typename TControl>
class TestInput final : public Source<TControl> {
 public:
  using Source<TControl>::emit;
};
using TestAxis = TestInput<Axis>;
using TestButton = TestInput<Button>;
using TestHat = TestInput<Hat>;

}// namespace fredemmott::inputmapping
