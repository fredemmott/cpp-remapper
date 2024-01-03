/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cpp-remapper/MappableOutput.h>
#include <cpp-remapper/SinkPtr.h>

#include <cstdint>

namespace fredemmott::inputmapping {

class FAVHIDDevice;

class MappableFAVHIDOutput final : public MappableOutput {
 private:
  struct Impl;
  // Must be initialized before the const members
  std::shared_ptr<Impl> p;

 public:
  MappableFAVHIDOutput() = delete;
  explicit MappableFAVHIDOutput(uint8_t vjoy_id);
  MappableFAVHIDOutput(std::shared_ptr<FAVHIDDevice> dev);
  ~MappableFAVHIDOutput();

  std::shared_ptr<OutputDevice> getDevice() const override;

  ButtonSinkPtr button(uint8_t id) const;
  HatSinkPtr hat(uint8_t id) const;

  const AxisSinkPtr XAxis, YAxis, ZAxis, RXAxis, RYAxis, RZAxis, Slider, Dial;

  const ButtonSinkPtr Button1, Button2, Button3, Button4, Button5, Button6,
    Button7, Button8, Button9, Button10, Button11, Button12, Button13, Button14,
    Button15, Button16, Button17, Button18, Button19, Button20, Button21,
    Button22, Button23, Button24, Button25, Button26, Button27, Button28,
    Button29, Button30, Button31, Button32, Button33, Button34, Button35,
    Button36, Button37, Button38, Button39, Button40, Button41, Button42,
    Button43, Button44, Button45, Button46, Button47, Button48, Button49,
    Button50, Button51, Button52, Button53, Button54, Button55, Button56,
    Button57, Button58, Button59, Button60, Button61, Button62, Button63,
    Button64, Button65, Button66, Button67, Button68, Button69, Button70,
    Button71, Button72, Button73, Button74, Button75, Button76, Button77,
    Button78, Button79, Button80, Button81, Button82, Button83, Button84,
    Button85, Button86, Button87, Button88, Button89, Button90, Button91,
    Button92, Button93, Button94, Button95, Button96, Button97, Button98,
    Button99, Button100, Button101, Button102, Button103, Button104, Button105,
    Button106, Button107, Button108, Button109, Button110, Button111, Button112,
    Button113, Button114, Button115, Button116, Button117, Button118, Button119,
    Button120, Button121, Button122, Button123, Button124, Button125, Button126,
    Button127, Button128;

  const HatSinkPtr Hat1, Hat2, Hat3, Hat4;
};
}// namespace fredemmott::inputmapping