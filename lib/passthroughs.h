/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"

namespace fredemmott::inputmapping {

/* You're unlikely to want to create these in a profile; they're created
 * automatically by the Mapper when an (Axis|Button|Hat)Target is specified,
 * to convert them to an (Axis|Button|Hat)Action
 */

class VJoyAxis : public AxisAction {
  public:
    VJoyAxis(const AxisTarget& target);
    void map(long value);
  private:
    AxisTarget mTarget;
};

class VJoyButton : public ButtonAction {
  public:
    VJoyButton(const ButtonTarget& target);
    void map(bool value);
  private:
    ButtonTarget mTarget;
};

class VJoyHat : public HatAction {
  public:
    VJoyHat(const HatTarget& target);
    void map(uint16_t value);
  private:
    HatTarget mTarget;
};

} // namespace fredemmott::inputmapping
