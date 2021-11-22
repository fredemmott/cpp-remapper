/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

/* This file is to:
 * - verify the header in isolation
 * - do some simple static tests
 */
#include "CompositeSink.h"

#include "SinkRef.h"

namespace {
using namespace fredemmott::inputmapping;

void static_test() {
  ButtonSinkRef b = [](bool) {};
  auto single_button = all(b);
  auto two_buttons = all(b, b);
  auto many_buttons = all(b, b, b, b, b);

  /* TODO:
  auto lambda = all([](bool) {});
  auto many_lambdas = all(
    [](bool) {},
    [](bool) {},
    [](bool) {}
  );
  */
}

}// namespace