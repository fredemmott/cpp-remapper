/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "Sink.h"
#include "Source.h"

namespace fredemmott::inputmapping {

/** Turn a momentary button into a latched/toggle button.
 */
class MomentaryToLatchedButton final : public Sink<Button>,
                                       public Source<Button> {
 private:
  bool mPressed = false;

 public:
  MomentaryToLatchedButton() = default;
  explicit MomentaryToLatchedButton(bool initiallyPressed);
  ~MomentaryToLatchedButton();

  virtual void map(Button::Value value) override;
};

}// namespace fredemmott::inputmapping
