/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "lib/CompositeSink.h"
#include "lib/Sink.h"
#include "lib/Source.h"
#include "lib/easymode.h"
#include "lib/render_axis.h"

using namespace fredemmott::inputmapping;

#include <cstdio>
#include <exception>

#define REQUIRE(x) \
  if (!(x)) { \
    printf( \
      "Expectation '%s' failed in %s() at %s:%d\n", \
      #x, \
      __func__, \
      __FILE__, \
      __LINE__); \
    __debugbreak(); \
    exit(1); \
  }

struct ProgressPrinter {
  ProgressPrinter(const char *name) : mName(name) {
    printf("test_%s...\n", name);
  }

  ~ProgressPrinter() {
    if (std::uncaught_exceptions()) {
      printf("... FAILED!\n", mName);
    } else {
      printf("... OK.\n", mName);
    }
  }

 private:
  const char *mName;
};

std::map<std::string, void (*)()> registered_tests;
class TestRegistration final {
 public:
	TestRegistration(const char *name, void (*impl)()) {
		registered_tests.emplace(name, impl);
	}
};
#define TEST_CASE(x) \
	void test_##x(); \
	TestRegistration test_##x##_reg_##__COUNTER__(#x "()", test_##x); \
	void test_##x()


template <typename TControl>
class TestInput : public ::fredemmott::inputmapping::Source<TControl> {
 public:
  using Source<TControl>::emit;
};
using TestAxis = TestInput<Axis>;
using TestButton = TestInput<Button>;
using TestHat = TestInput<Hat>;

TEST_CASE(value_ptr) {
  long out = -1;
  TestAxis axis;
  axis >> &out;
  axis.emit(123);
  REQUIRE(out == 123);
}

TEST_CASE(lambdas) {
  long out = -1;
  TestAxis axis;
  axis >> &out;
  axis.emit(1337);
  REQUIRE(out == 1337);

  axis >> [](Axis::Value value) { return Axis::MAX - value; } >> &out;
  axis.emit(123);
  REQUIRE(out == Axis::MAX - 123);
  axis.emit(Axis::MAX - 456);
  REQUIRE(out == 456);
}

TEST_CASE(source_sink_transform) {
  // Testing the plumbing of >>, not the specific transform
  long out = -1;
  TestAxis axis;
  SquareDeadzone dz(10);

  // Via pointer
  axis >> &dz >> &out;
  axis.emit(0);
  REQUIRE(out == 0);
  axis.emit(Axis::MAX);
  REQUIRE(out == Axis::MAX);
  axis.emit(100);
  REQUIRE(out > 100);
  axis.emit(Axis::MAX - 100);
  REQUIRE(out < Axis::MAX - 100);

  // Via temporary
  axis >> SquareDeadzone {10} >> &out;
  axis.emit(0);
  REQUIRE(out == 0);
  axis.emit(Axis::MAX);
  REQUIRE(out == Axis::MAX);
}

void test_square_deadzone_impl(uint8_t percent) {
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

TEST_CASE(small_square_deadzone) {
  test_square_deadzone_impl(10);
}

TEST_CASE(large_square_deadzone) {
  test_square_deadzone_impl(90);
}

TEST_CASE(axis_curve) {
  long out = -1;

  render_axis("linear.bmp", AxisCurve {0});
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

  render_axis("extreme.bmp", AxisCurve {0.99});
  TestAxis extreme;
  extreme >> AxisCurve {0.99} >> &out;

  extreme.emit(0);
  REQUIRE(out == 0);
  extreme.emit(0xffff);
  REQUIRE(out == 0xffff);
  extreme.emit(0x7fff);
  REQUIRE(out == 0x7fff);
  extreme.emit(0x4000);
  REQUIRE(out > 0x4000 && out < 0x7fff);
  extreme.emit(0x7fff + 0x4000);
  REQUIRE(out > 0x7fff && out < (0x7ffff + 0x4000));

  render_axis("gentle.bmp", AxisCurve {0.5});
  TestAxis gentle;
  gentle >> AxisCurve {0.5} >> &out;
  extreme.emit(0x4000);
  const auto extreme_out = out;
  gentle.emit(0x4000);
  REQUIRE(out > 0x4000 && out < 0x7fff);
  // As 0x4000 is less than the midpoint, the smaller number has the most
  // deflection
  REQUIRE(out < extreme_out);
}

TEST_CASE(bind_to_sink) {
  bool b1 = false, b2 = false;
  TestAxis axis;
  axis >> AxisToButtons {
    {0, 10, [&b1](bool value) { b1 = value; }},
    {90, 100, [&b2](bool value) { b2 = value; }}};
  axis.emit(0);
  REQUIRE(b1);
  REQUIRE(!b2);
  axis.emit(Axis::MAX / 10);
  REQUIRE(b1);
  REQUIRE(!b2);
  axis.emit(Axis::MAX / 2);
  REQUIRE(!b1);
  REQUIRE(!b2);
  axis.emit(Axis::MAX);
  REQUIRE(!b1);
  REQUIRE(b2);
}

TEST_CASE(bind_to_multi) {
  TestAxis axis;
  // 1, 2, n are special
  axis >> [](long) {};
  axis >> ::fredemmott::inputmapping::all([](long) {}, [](long) {});
}

TEST_CASE(axis_to_hat) {
  TestAxis x, y;
  Hat::Value hat;
  AxisToHat ath;
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

int main() {
  printf("----- Starting Test Run -----\n");
  for (const auto& [name, test]: registered_tests) {
    ProgressPrinter pp(name.c_str());
    test();
  }
  printf("All tests passed.\n");
  return 0;
}
