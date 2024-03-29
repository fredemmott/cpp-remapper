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

#include <cpp-remapper/MappableOutput.h>
#include <cpp-remapper/SinkPtr.h>

namespace fredemmott::inputmapping {

class DS4Device;

class MappableDS4Output final : public MappableOutput {
 public:
  explicit MappableDS4Output();
  MappableDS4Output(std::shared_ptr<DS4Device> dev);
  ~MappableDS4Output();

  std::shared_ptr<OutputDevice> getDevice() const override;

  const AxisSinkPtr LXAxis, LYAxis, LTrigger, RXAxis, RYAxis, RTrigger;

  const ButtonSinkPtr ButtonTriangle, ButtonCircle, ButtonCross, ButtonSquare,
    ButtonLTrigger, ButtonRTrigger, ButtonLShoulder, ButtonRShoulder,
    ButtonOptions, ButtonShare;

  const ButtonSinkPtr ButtonPS, ButtonTouchPad;

  const ButtonSinkPtr ButtonDPadNorth, ButtonDPadNorthEast, ButtonDPadEast,
    ButtonDPadSouthEast, ButtonDPadSouth, ButtonDPadSouthWest, ButtonDPadWest,
    ButtonDPadNorthWest;

 private:
  std::shared_ptr<DS4Device> mDevice;
};

}// namespace fredemmott::inputmapping
