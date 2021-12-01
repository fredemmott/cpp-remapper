/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "AxisToHat.h"

#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("AxisToHat") {
  TestAxis x, y;
  Hat::Value hat;
  AxisToHat ath;
  // TODO: tuple assignments?
  x >> ath.XAxis;
  y >> ath.YAxis;
  ath >> &hat;

  // verify that both axis default to cente
  x.emit(Axis::MID);
  REQUIRE(hat == Hat::CENTER);
  ath = AxisToHat();
  ath >> &hat;
  x.emit(Axis::MID);
  REQUIRE(hat == Hat::CENTER);

  y.emit(Axis::MIN);
  REQUIRE(hat == Hat::NORTH);

  x.emit(Axis::MIN);
  REQUIRE(hat == Hat::NORTH_WEST);

  x.emit(Axis::MAX);
  REQUIRE(hat == Hat::NORTH_EAST);

  y.emit(Axis::MID);
  REQUIRE(hat == Hat::EAST);

  x.emit(Axis::MID);
  REQUIRE(hat == Hat::CENTER);

  x.emit(Axis::MIN);
  REQUIRE(hat == Hat::WEST);

  y.emit(Axis::MAX);
  REQUIRE(hat == Hat::SOUTH_WEST);

  x.emit(Axis::MID);
  REQUIRE(hat == Hat::SOUTH);

  x.emit(Axis::MAX);
  REQUIRE(hat == Hat::SOUTH_EAST);
}
