/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "ButtonToAxis.h"

namespace fredemmott::inputmapping {

void ButtonToAxis::map(bool value) {
  emit(value ? 65535 : 0);
}

}// namespace fredemmott::inputmapping
