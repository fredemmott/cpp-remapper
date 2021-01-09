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

void test_square_deadzone() {
  START_TEST;
  long out = 12345;

  SquareDeadzone small_dz(10, [&out](long value) { out = value; });
  small_dz.map(0x7fff); // mid point
  // In deadzone
  REQUIRE(out == 0x7fff);
  small_dz.map(0x8000);
  REQUIRE(out == 0x7fff);
  small_dz.map(0x7ffe);
  REQUIRE(out == 0x7fff);
  // Out of deadzone: extremes
  small_dz.map(0);
  REQUIRE(out == 0);
  small_dz.map(0xffff);
  REQUIRE(out == 0xffff);
  // Return to deadzone
  small_dz.map(0x80ff);
  REQUIRE(out == 0x7fff);

  // Not extreme; as the output should still cover the full range, there's
  // scaling, so 1 change in input means *at least* one change in output

  // Near max
  small_dz.map(0xfffe);
  REQUIRE(out == 0xfffd);
  // Just above center
  const long small_dz_pos = ceil(0.1 * 0x7fff);
  small_dz.map(0x7fff + small_dz_pos);
  REQUIRE(out == 0x7fff);
  small_dz.map(0x7fff + small_dz_pos + 1);
  REQUIRE(out == 0x8000);
  // Just below center
  const long small_dz_neg = floor(0.1 * 0x7fff);
  small_dz.map(0x7fff - small_dz_neg);
  REQUIRE(out == 0x7fff);
  small_dz.map(0x7fff - (small_dz_neg + 1));
  REQUIRE(out == 0x7ffe);
  // Near min
  small_dz.map(1);
  REQUIRE(out == 1);

  // Half way points
  small_dz.map(0x7fff + 0x4000 + (small_dz_pos / 2) + 1);
  REQUIRE(out == 0x7fff + 0x4000);
  small_dz.map(0x7fff - 0x4000 - (small_dz_neg / 2));
  REQUIRE(out == 0x7fff - 0x4000);
}

int main() {
  test_square_deadzone();
  return 0;
}
