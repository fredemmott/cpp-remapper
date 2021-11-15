/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "XUSBDevice.h"

namespace fredemmott::inputmapping {

class XUSBDevice::Impl {};

void XUSBDevice::flush() {}

XUSBDevice* XUSBDevice::setButton(Button button, bool value) { return this; }
XUSBDevice* XUSBDevice::setLX(long value) { return this; }
XUSBDevice* XUSBDevice::setLY(long value) { return this; }
XUSBDevice* XUSBDevice::setRX(long value) { return this; }
XUSBDevice* XUSBDevice::setRY(long value) { return this; }
XUSBDevice* XUSBDevice::setLeftTrigger(long value) { return this; }
XUSBDevice* XUSBDevice::setRightTrigger(long value) { return this; }

} // namespace fredemmott::inputmapping
