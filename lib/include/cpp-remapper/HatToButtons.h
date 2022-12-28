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

#include <cpp-remapper/Sink.h>
#include <cpp-remapper/SinkPtr.h>

namespace fredemmott::inputmapping {

class MappableVJoyOutput;

class HatToButtons final : public Sink<Hat> {
 public:
  enum class Interpolation {
    // divide range by number of buttons, only press button when hat is pointing
    // *exactly* at it - e.g. 0 degrees == press button 1, 1 degree and 359 degrees
    // == press nothing
    None,
    // fill the range, and interpolate. e.g. if there are 4 buttons, if hat is
    // pointing North East, press both buttons 1 and 2
    MultiPress,
  };
  class CenterButton {
   private:
    ButtonSinkPtr mButton;

   public:
    // clang-format off
    template <typename T>
    requires
      (!std::derived_from<std::decay_t<T>, CenterButton>) // needed by clang
      && convertible_to_sink_ptr<T, Button>
      // clang-format on
      explicit CenterButton(T&& button)
      : mButton(convert_to_any_sink_ptr(std::forward<T>(button))) {
    }

    const ButtonSinkPtr& get() const;
  };
  static_assert(!convertible_to_any_sink_ptr<CenterButton>);

  HatToButtons() = delete;

  HatToButtons(MappableVJoyOutput* output, uint8_t first, uint8_t count);

  HatToButtons(
    const CenterButton& center,
    MappableVJoyOutput* output,
    uint8_t first,
    uint8_t count);

  // clang-format off
  template <typename First, convertible_to_sink_ptr<Button>... Rest>
  requires
    (!std::derived_from<std::decay_t<First>, HatToButtons>) // needed by clang
    && convertible_to_sink_ptr<First, Button>
  // clang-format on
  HatToButtons(First&& first, Rest&&... rest) {
    mButtons = {
      convert_to_any_sink_ptr(std::forward<First>(first)),
      convert_to_any_sink_ptr(std::forward<Rest>(rest))...};
  }

  template <
    convertible_to_sink_ptr<Button> First,
    convertible_to_sink_ptr<Button>... Rest>
  HatToButtons(const CenterButton& center, First&& first, Rest&&... rest) {
    mCenter = center.get();
    mButtons = {
      convert_to_any_sink_ptr(std::forward<First>(first)),
      convert_to_any_sink_ptr(std::forward<Rest>(rest))...};
  }

  template <class... Rest>
  HatToButtons(Interpolation interpolation, Rest&&... rest)
    : HatToButtons(std::forward<Rest>(rest)...) {
    mInterpolation = interpolation;
  }

  virtual void map(Hat::Value value) override;

 private:
  Interpolation mInterpolation = Interpolation::MultiPress;
  std::optional<ButtonSinkPtr> mCenter;
  std::vector<ButtonSinkPtr> mButtons;

  void assignToVJoy(MappableVJoyOutput* output, uint8_t first, uint8_t count);
};

// Given we're doing funky template overloads for the constructors, let's make
// sure that we've not accidentally defined an implicit conversion path
static_assert(!convertible_to_sink_ptr<HatToButtons, Axis>);
static_assert(!convertible_to_sink_ptr<HatToButtons, Button>);
static_assert(convertible_to_sink_ptr<HatToButtons, Hat>);

}// namespace fredemmott::inputmapping
