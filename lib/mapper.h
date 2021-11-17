/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"
#include "eventhandler.h"

#include <chrono>
#include <functional>
#include <ratio>
#include <map>

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
      void setOutputs(const std::vector<OutputDevice*>& to_flush);

      /// Call setOutputs first
      void run();

      void passthrough(const MappableInput&, const MappableVJoyOutput&);

      void map(const AxisSource& source, const AxisEventHandler& handler);
      void map(const ButtonSource& source, const ButtonEventHandler& handler);
      void map(const HatSource& source, const HatEventHandler& handler);

      struct ButtonMapping {
        ButtonSource source;
        ButtonEventHandler action;
      };
      struct HatMapping {
        HatSource source;
        HatEventHandler action;
      };
      struct AxisMapping {
        AxisSource source;
        AxisEventHandler action;
      };
      void map(const std::initializer_list<AxisMapping>& rules);
      void map(const std::initializer_list<ButtonMapping>& rules);
      void map(const std::initializer_list<HatMapping>& rules);

      static void inject(
        const std::chrono::steady_clock::duration& delay,
        const std::function<void()>& handler
      );
    private:
      struct DeviceMappings {
        std::map<uint8_t, AxisEventHandler> axes;
        std::map<uint8_t, ButtonEventHandler> buttons;
        std::map<uint8_t, HatEventHandler> hats;
      };
      std::map<gameinput::InputDevice*, DeviceMappings> mMappings;
      std::vector<OutputDevice*> mToFlush;
      std::map<void*, std::function<void()>> mInjected;

      void flush();
  };
} // namespace fredemmott::inputmapping
