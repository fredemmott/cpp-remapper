/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "lib/easymode.h"
#include "lib/render_axis.h"

using fredemmott::inputmapping::Axis;
using fredemmott::inputmapping::Button;
using fredemmott::inputmapping::Control;
using fredemmott::inputmapping::Hat;
using fredemmott::inputmapping::Sink;
using fredemmott::inputmapping::Source;

using fredemmott::inputmapping::render_axis;

#include <cstdio>
#include <exception>

#define REQUIRE(x) \
  if (!(x)) { \
    printf( \
      "Expectation '%s' failed in %s() at line %d\n", #x, __func__, __LINE__); \
    __debugbreak(); \
    exit(1); \
  }

struct ProgressPrinter {
  ProgressPrinter(const char *name) : mName(name) {
    printf("Starting test %s()...\n", name);
  }

  ~ProgressPrinter() {
    if (std::uncaught_exceptions()) {
      printf("FAILED: %s()\n", mName);
    } else {
      printf("OK: %s()\n", mName);
    }
  }

 private:
  const char *mName;
};
#define START_TEST ProgressPrinter pp_##__COUNTER__(__func__)

template <std::derived_from<Control> TControl>
class TestInput : public ::fredemmott::inputmapping::Source<TControl> {
 public:
  using Source<TControl>::emit;
};
using TestAxis = TestInput<Axis>;
using TestButton = TestInput<Button>;
using TestHat = TestInput<Hat>;

void test_value_ptr() {
  START_TEST;
  long out = -1;
  TestAxis axis;
  axis >> &out;
  axis.emit(123);
  REQUIRE(out == 123);
}

void test_lambdas() {
  START_TEST;
  long out = -1;
  TestAxis axis;
  axis >> &out;
  axis.emit(1337);
  REQUIRE(out == 1337);

  // TODO one line
  auto step = axis >> [](Axis::Value value) { return Axis::MAX - value; };
  step  >> &out;
  axis.emit(123);
  REQUIRE(out == Axis::MAX - 123);
  axis.emit(Axis::MAX - 456);
  REQUIRE(out == 456);
}

void test_source_sink_transform() {
  START_TEST;
  // Testing the plumbing of >>, not the specific transform
  long out = -1;
  TestAxis axis;
  SquareDeadzone dz(10);

  // Via pointer
  //axis >> ::fredemmott::inputmapping::UnsafeRef<SquareDeadzone>(&dz)
  ::fredemmott::inputmapping::UnsafeRef<Source<Axis>> axisRef(&axis);
  auto ret = axisRef >> ::fredemmott::inputmapping::UnsafeRef<SquareDeadzone>(&dz);
  ret = axis >> ::fredemmott::inputmapping::UnsafeRef<SquareDeadzone>(&dz);
  axis >> &dz;
  //TODO axis >> &dz >> &out;
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

void test_small_square_deadzone() {
  START_TEST;
  test_square_deadzone_impl(10);
}

void test_large_square_deadzone() {
  START_TEST;
  test_square_deadzone_impl(90);
}

void test_axis_curve() {
  START_TEST;
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

void test_bind_to_sink() {
  START_TEST;
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

void static_test_vjoy() {
  // Input device is irrelevant, but we currently need one.
  auto [p, _, vj1] = create_profile(VPC_RIGHT_WARBRD, VJOY_1);
}

void static_test_vigem() {
  // Input device is irrelevant, but we currently need one.
  auto [p, _, xpad] = create_profile(VPC_RIGHT_WARBRD, VIGEM_X360_PAD_1);
}

int main() {
  printf("----- Starting Test Run -----\n");
  test_value_ptr();
  test_lambdas();
  test_bind_to_sink();
  test_source_sink_transform();
  test_small_square_deadzone();
  test_large_square_deadzone();
  test_axis_curve();

  printf("All tests passed.\n");
  return 0;
}
