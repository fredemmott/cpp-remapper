/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "AxisCurve.h"
#include "tests.h"

using namespace fredemmott::inputmapping;

TEST_CASE("AxisCurve") {
	long out = -1;

	TestAxis linear;
	linear >> AxisCurve {0} >> &out;

	linear.emit(0);
	REQUIRE(out == 0);
	linear.emit(0xffff);
	REQUIRE(out == 0xffff);
	linear.emit(0x7fff);
	REQUIRE(out == 0x7fff);
	linear.emit(0x1234);
	REQUIRE(out == 0x1234);

	TestAxis extreme;
	extreme >> AxisCurve {0.99} >> &out;

	extreme.emit(0);
	REQUIRE(out == 0);
	extreme.emit(0xffff);
	REQUIRE(out == 0xffff);
	extreme.emit(0x7fff);
	REQUIRE(out == 0x7fff);
	extreme.emit(0x4000);
	REQUIRE((out > 0x4000 && out < 0x7fff));
	extreme.emit(0x7fff + 0x4000);
	REQUIRE((out > 0x7fff && out < (0x7ffff + 0x4000)));

	TestAxis gentle;
	gentle >> AxisCurve {0.5} >> &out;
	extreme.emit(0x4000);
	const auto extreme_out = out;
	gentle.emit(0x4000);
	REQUIRE((out > 0x4000 && out < 0x7fff));
	// As 0x4000 is less than the midpoint, the smaller number has the most
	// deflection
	REQUIRE(out < extreme_out);
}
