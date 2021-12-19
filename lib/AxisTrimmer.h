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
#include "Source.h"

namespace fredemmott::inputmapping {

/** Recenter an axis by pressing and holding a button.
 *
 * Any movement is ignored while the button is pressed; trim is adjusted by the
 * position difference between where the button was pressed and where it was
 * released.
 *
 * For example, if you move the joystick to (100, 100), press the button, center
 * the stick, then release the button, when centered, the joystick will report
 * (100, 100); if you then move to (100, 100), it will report (200, 200).
 *
 * Trim is limited to half of the axis range.
 */
class AxisTrimmer final : public Source<Axis>, public Sink<Axis> {
 private:
 public:
  const ButtonSinkPtr ResetButton = [this](bool _) { reset(); };
  const ButtonSinkPtr TrimButton = [this](bool pressed) { trim(pressed); };

  void map(Axis::Value v) override;

 private:
  void update();
  void trim(bool pressed);
  void reset();

  Axis::Value mValue = Axis::MID;
  Axis::Value mOffset = 0;

  bool mTrimming = false;
  Axis::Value mStartValue = 0;
};

}// namespace fredemmott::inputmapping
