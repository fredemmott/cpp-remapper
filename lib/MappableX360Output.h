/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"
#include "eventhandler.h"
#include "MappableOutput.h"
#include "X360Device.h"

#include <cstdint>
#include <memory>

namespace fredemmott::inputmapping {

class MappableX360Output final : public MappableOutput {
public:
  explicit MappableX360Output();
  MappableX360Output(std::shared_ptr<X360Device> dev);

  X360Device* getDevice() const override;

  const AxisEventHandler
    LXAxis, LYAxis, LTrigger,
    RXAxis, RYAxis, RTrigger;

  const ButtonEventHandler
    ButtonA, ButtonB, ButtonX, ButtonY,
    ButtonBack, ButtonStart, ButtonGuide,
    DPadUp, DPadDown, DPadLeft, DPadRight,
    ButtonLStick, ButtonRStick,
    ButtonLShoulder, ButtonRShoulder;
  private:
   std::shared_ptr<X360Device> mDevice;
};

} // namespace fredemmott::inputmapping
