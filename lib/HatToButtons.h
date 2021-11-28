/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "Sink.h"
#include "SinkRef.h"

namespace fredemmott::inputmapping {

class MappableVJoyOutput;

class HatToButtons final : public Sink<Hat> {
 private:
  std::optional<ButtonSinkRef> mCenter;
  std::vector<ButtonSinkRef> mButtons;

 public:
  HatToButtons() = delete;

  HatToButtons(MappableVJoyOutput* output, uint8_t first, uint8_t count);
  HatToButtons(
    const ButtonSinkRef& center,
    MappableVJoyOutput* output,
    uint8_t first,
    uint8_t count);
  HatToButtons(const std::vector<ButtonSinkRef>& buttons);
  HatToButtons(
    const ButtonSinkRef& center,
    const std::vector<ButtonSinkRef>& buttons);

  virtual void map(Hat::Value value) override;

 private:
  void assignToVJoy(MappableVJoyOutput* output, uint8_t first, uint8_t count);
};

}// namespace fredemmott::inputmapping