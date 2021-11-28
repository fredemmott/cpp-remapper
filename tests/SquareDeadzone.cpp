/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "squaredeadzone.h"
#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("SquareDeadzone"){
  const auto percent = GENERATE(10, 90);

	long out = -1;
	TestAxis axis;
	axis >> SquareDeadzone(percent) >> &out;

	axis.emit(0x7fff);// mid point
	// In deadzone
	REQUIRE(out == 0x7fff);
	axis.emit(0x8000);
	REQUIRE(out == 0x7fff);
	axis.emit(0x7ffe);
	REQUIRE(out == 0x7fff);
	// Out of deadzone: extremes
	axis.emit(0);
	REQUIRE(out == 0);
	axis.emit(0xffff);
	REQUIRE(out == 0xffff);
	// Return to deadzone
	axis.emit(0x80ff);
	REQUIRE(out == 0x7fff);

	const long dz_size = round(0x8000 * percent / 100.0);
	// 1 change in input produces this size change in output...
	const double out_delta = 0x8000 * 1.0 / (0x8000 - dz_size);
	// ... but as we're using < and > constraints with a input of 1, we always
	// want the ceil
	const long delta_ceil = ceil(out_delta);

	// Just above center
	axis.emit(0x7fff + dz_size);
	REQUIRE(out == 0x7fff);
	axis.emit(0x7fff + dz_size + 1);
	REQUIRE(out > 0x7fff);
	REQUIRE(out <= 0x7fff + delta_ceil);
	// Near max
	axis.emit(0xfffe);
	REQUIRE(out == 0xffff - delta_ceil);
	// Just below center
	axis.emit(0x7fff - dz_size + 1);
	REQUIRE(out == 0x7fff);
	axis.emit(0x7fff - dz_size);
	REQUIRE(out < 0x7fff);
	REQUIRE(out >= 0x7fff - delta_ceil);
	// Near min
	axis.emit(1);
	REQUIRE(out >= 1);
	REQUIRE(out <= delta_ceil);

	// Half way points
	axis.emit(0x7fff + 0x4000 + (dz_size / 2) + 1);
	REQUIRE(out >= 0x7fff + 0x4000);
	REQUIRE(out <= 0x7fff + 0x4000 + delta_ceil);
	axis.emit(0x7fff - 0x4000 - (dz_size / 2));
	REQUIRE(out <= 0x7fff - 0x4000);
	REQUIRE(out >= 0x7fff - 0x4000 - delta_ceil);
}
