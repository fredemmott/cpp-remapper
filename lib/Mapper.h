/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace fredemmott::inputmapping {
class InputDevice;
}

namespace fredemmott::inputmapping {
class EventSink;
class EventSource;
class MappableInput;
class MappableOutput;
class MappableVJoyOutput;
class OutputDevice;

class Mapper {
 public:
  void setDevices(
    const std::vector<MappableInput>& inputs,
    const std::vector<std::shared_ptr<OutputDevice>>& outputs);

  /// Call setDevices first
  void run();

  void passthrough(MappableInput&, const MappableVJoyOutput&);

  static void inject(
    const std::chrono::steady_clock::duration& delay,
    const std::function<void()>& handler);

 private:
  std::vector<std::shared_ptr<EventSource>> mEventSources;
  std::vector<std::shared_ptr<EventSink>> mEventSinks;
  std::map<void*, std::function<void()>> mInjected;

  void flush();
};
}// namespace fredemmott::inputmapping
