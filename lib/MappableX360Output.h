/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cstdint>
#include <memory>

#include "MappableOutput.h"
#include "X360Device.h"
#include "actionsapi.h"

namespace fredemmott::inputmapping {

class MappableX360Output final : public MappableOutput {
 public:
  explicit MappableX360Output();
  MappableX360Output(std::shared_ptr<X360Device> dev);
  ~MappableX360Output();

  std::shared_ptr<OutputDevice> getDevice() const override;

  const AxisSinkRef LXAxis, LYAxis, LTrigger, RXAxis, RYAxis, RTrigger;

  const ButtonSinkRef ButtonA, ButtonB, ButtonX, ButtonY, ButtonBack,
    ButtonStart, ButtonGuide, DPadUp, DPadDown, DPadLeft, DPadRight,
    ButtonLStick, ButtonRStick, ButtonLShoulder, ButtonRShoulder;

 private:
  std::shared_ptr<X360Device> mDevice;
};

}// namespace fredemmott::inputmapping