/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "lib/easymode.h"

#include <cstdio>

#define REQUIRE(x) \
  if (!(x)) { \
    printf("Expectation '%s' failed in %s() at line %d\n", #x, __func__, __LINE__); \
    exit(1); \
  }

struct ProgressPrinter{
  ProgressPrinter(const char* name): mName(name) {
    printf("Starting test %s()...\n", name);
  }

  ~ProgressPrinter() {
    if (std::uncaught_exception()) {
      printf("FAILED: %s()\n", mName);
    } else {
      printf("OK: %s()\n", mName);
    }
  }

  private:
    const char* mName;
};
#define START_TEST ProgressPrinter pp_ ## __COUNTER__ (__func__)

void test_square_deadzone_impl(uint8_t percent) {
  long out = -1;

  SquareDeadzone dz(percent, [&out](long value) { out = value; });
  dz.map(0x7fff); // mid point
  // In deadzone
  REQUIRE(out == 0x7fff);
  dz.map(0x8000);
  REQUIRE(out == 0x7fff);
  dz.map(0x7ffe);
  REQUIRE(out == 0x7fff);
  // Out of deadzone: extremes
  dz.map(0);
  REQUIRE(out == 0);
  dz.map(0xffff);
  REQUIRE(out == 0xffff);
  // Return to deadzone
  dz.map(0x80ff);
  REQUIRE(out == 0x7fff);

  const long dz_size = round(0x8000 * percent / 100.0);
  // 1 change in input produces this size change in output...
  const double out_delta = 0x8000 * 1.0 / (0x8000 - dz_size);
  // ... but as we're using < and > constraints with a input of 1, we always
  // want the ceil
  const long delta_ceil = ceil(out_delta);

  // Just above center
  dz.map(0x7fff + dz_size);
  REQUIRE(out == 0x7fff);
  dz.map(0x7fff + dz_size + 1);
  REQUIRE(out > 0x7fff);
  REQUIRE(out <= 0x7fff + delta_ceil);
  // Near max
  dz.map(0xfffe);
  REQUIRE(out == 0xffff - delta_ceil);
  // Just below center
  dz.map(0x7fff - dz_size + 1);
  REQUIRE(out == 0x7fff);
  dz.map(0x7fff - dz_size);
  REQUIRE(out < 0x7fff);
  REQUIRE(out >= 0x7fff - delta_ceil);
  // Near min
  dz.map(1);
  REQUIRE(out >= 1);
  REQUIRE(out <= delta_ceil);

  // Half way points
  dz.map(0x7fff + 0x4000 + (dz_size / 2) + 1);
  REQUIRE(out >= 0x7fff + 0x4000);
  REQUIRE(out <= 0x7fff + 0x4000 + delta_ceil);
  dz.map(0x7fff - 0x4000 - (dz_size/ 2));
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

int main() {
  test_small_square_deadzone();
  test_large_square_deadzone();
  printf("All tests passed.\n");
  return 0;
}
