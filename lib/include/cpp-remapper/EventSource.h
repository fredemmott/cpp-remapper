/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace fredemmott::inputmapping {

class EventSource {
 protected:
  EventSource();

 public:
  virtual ~EventSource();
  virtual HANDLE getHandle() = 0;
  virtual void poll() = 0;
};
}// namespace fredemmott::inputmapping
