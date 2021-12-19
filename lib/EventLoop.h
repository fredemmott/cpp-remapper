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
class EventSink;
class EventSource;

class EventLoop final {
 public:
  void setEventSinks(const std::vector<std::shared_ptr<EventSink>>& sinks);
  void setEventSources(
    const std::vector<std::shared_ptr<EventSource>>& sources);

  void run();

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
