/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#define CATCH_CONFIG_MAIN

// Don't intercept ASAN or standard library exceptions
#define CATCH_CONFIG_NO_WINDOWS_SEH

#include <catch.hpp>
