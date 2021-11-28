/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "HatToButtons.h"

#include "MappableVJoyOutput.h"

namespace fredemmott::inputmapping {

HatToButtons::HatToButtons(
  MappableVJoyOutput* output,
  uint8_t first,
  uint8_t count) {
  assignToVJoy(output, first, count);
}

HatToButtons::HatToButtons(
  const ButtonSinkRef& center,
  MappableVJoyOutput* output,
  uint8_t first,
  uint8_t count)
  : mCenter(center) {
  assignToVJoy(output, first, count);
}

HatToButtons::HatToButtons(const std::vector<ButtonSinkRef>& buttons)
  : mButtons(buttons) {
}

HatToButtons::HatToButtons(
  const ButtonSinkRef& center,
  const std::vector<ButtonSinkRef>& buttons)
  : mCenter(center), mButtons(buttons) {
}

void HatToButtons::map(Hat::Value value) {
  if (mCenter) {
    (*mCenter)->map(value == Hat::CENTER);
  }
  if (value == Hat::CENTER) {
    for (auto& next: mButtons) {
      next->map(false);
    }
    return;
  }

  const auto step = 36000 / mButtons.size();
  // Let's say we have 4 buttons: N, E, S, W
  // If the hat is at NE, we want both N and E on, so there's an area that's
  // "NE" - which goes half way from true NE to E - in another words, the N
  // button is pressed when the hat is between WNW and ENE, and covers 3/8ths
  // of the area
  const auto range = (36000 * 3) / (mButtons.size() * 2);
  // first one is special, as North should be on for a bit both sides of 0
  auto offset = range / 2;
  mButtons[0]->map(value <= offset || value > 36000 - offset);

  // If we have 4 buttons, the 'E' button starts pressed active at NNE, while
  // 'N' is still active. N drops off 45 degrees later at ENE
  offset = 36000 / (mButtons.size() * 4);
  for (auto i = 1; i < mButtons.size(); ++i) {
    mButtons[i]->map(value > offset && value <= offset + range);
    offset += step;
  }
}

void HatToButtons::assignToVJoy(
  MappableVJoyOutput* output,
  uint8_t first,
  uint8_t count) {
  for (auto i = first; i < first + count; ++i) {
    mButtons.push_back(output->button(i));
  }
}

}// namespace fredemmott::inputmapping