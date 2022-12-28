/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cpp-remapper/AnyOfButton.h>
#include <cpp-remapper/AxisCurve.h>
#include <cpp-remapper/AxisToButtons.h>
#include <cpp-remapper/AxisToHat.h>
#include <cpp-remapper/AxisTrimmer.h>
#include <cpp-remapper/ButtonToAxis.h>
#include <cpp-remapper/CompositeSink.h>
#include <cpp-remapper/EventLoop.h>
#include <cpp-remapper/HatToButtons.h>
#include <cpp-remapper/LatchedToMomentaryButton.h>
#include <cpp-remapper/MomentaryToLatchedButton.h>
#include <cpp-remapper/Profile.h>
#include <cpp-remapper/Shift.h>
#include <cpp-remapper/ShortPressLongPress.h>
#include <cpp-remapper/SquareDeadzone.h>
#include <cpp-remapper/connections.h>
#include <cpp-remapper/devicedb.h>

using namespace fredemmott::inputmapping::devicedb;
using namespace fredemmott::inputmapping;
