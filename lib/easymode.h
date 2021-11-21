/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actions.h"
#include "connections.h"
#include "devicedb.h"
#include "mapper.h"
#include "profile.h"

using namespace fredemmott::gameinput::devicedb;
using namespace fredemmott::inputmapping::actions;
using namespace fredemmott::inputmapping::vigemids;
using namespace fredemmott::inputmapping::vjoyids;
using fredemmott::gameinput::HardwareID;
using fredemmott::gameinput::InstanceID;
using fredemmott::gameinput::VIDPID;
using fredemmott::inputmapping::Axis;
using fredemmott::inputmapping::Button;
using fredemmott::inputmapping::Hat;
using fredemmott::inputmapping::Profile;

using fredemmott::inputmapping::create_profile;
