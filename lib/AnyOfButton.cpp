/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "AnyOfButton.h"

namespace fredemmott::inputmapping {

void AnyOfButton::map(bool pressed) {
  if (pressed) {
    mPressed++;
    if (mPressed == 1) {
      emit(true);
    }
    return;
  }

  mPressed--;
  if (mPressed == 0) {
    emit(false);
  }
}

}// namespace fredemmott::inputmapping
