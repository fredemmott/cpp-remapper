/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "Sink.h"
#include "SinkPtr.h"

namespace fredemmott::inputmapping {

template <control T>
class Shift : public Sink<T> {
 private:
  bool* mShifted = nullptr;
  SinkPtr<T> mA, mB;

 public:
  template <convertible_to_sink_ptr<T> A, convertible_to_sink_ptr<T> B>
  Shift(bool* shifted, A&& a, B&& b)
    : mShifted(shifted),
      mA(convert_to_any_sink_ptr(std::forward<A>(a))),
      mB(convert_to_any_sink_ptr(std::forward<B>(b))) {
  }

  virtual void map(typename T::Value value) override {
    auto next = *mShifted ? mB : mA;
    next->map(value);
  }
};

// Help template inference along a bit...

template <convertible_to_sink_ptr<Axis> A, convertible_to_sink_ptr<Axis> B>
Shift(bool*, A, B) -> Shift<Axis>;

template <convertible_to_sink_ptr<Button> A, convertible_to_sink_ptr<Button> B>
Shift(bool*, A, B) -> Shift<Button>;

template <convertible_to_sink_ptr<Hat> A, convertible_to_sink_ptr<Hat> B>
Shift(bool*, A, B) -> Shift<Hat>;

}// namespace fredemmott::inputmapping
