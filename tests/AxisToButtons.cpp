/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "axistobuttons.h"

#include "CompositeSink.h"
#include "MappableVJoyOutput.h"
#include "tests.h"

// TODO: runtime test

using namespace fredemmott::inputmapping;

namespace {

// Check that the compiler lets us call it as intended
void static_test() {
  MappableVJoyOutput o(nullptr);
  AxisToButtons::Range range1 {0, 0, o.Button1};

  AxisToButtons::Range range2 {0, 0, all(o.Button1, o.Button2)};
  AxisToButtons foo {
    {0, 0, o.Button1}, {0, 0, all(o.Button1, o.Button2)}, {100, 100, [](bool) {
                                                           }}};
}

}// namespace
