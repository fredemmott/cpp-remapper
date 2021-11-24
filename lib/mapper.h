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
#include <memory>
#include <map>
#include <vector>

namespace fredemmott::gameinput {
class InputDevice;
}

namespace fredemmott::inputmapping {
struct MappableInput;
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
  std::vector<MappableInput> mInputs;
  std::vector<std::shared_ptr<OutputDevice>> mOutputs;
  std::map<void*, std::function<void()>> mInjected;

  void flush();
};
}// namespace fredemmott::inputmapping
