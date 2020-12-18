#pragma once

#include "actionsapi.h"
#include "eventhandler.h"

#include <map>

namespace fredemmott::gameinput {
  class InputDevice;
}

namespace fredemmott::vjoypp {
  class OutputDevice;
}

namespace fredemmott::inputmapping {
  struct MappableInput;
  struct MappableOutput;

  class Mapper {
    public:
      void setOutputs(const std::vector<MappableOutput>& to_flush);

      /// Call setOutputs first
      void run();

      void passthrough(const MappableInput&, const MappableOutput&);

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

    private:
      struct DeviceMappings {
        std::map<uint8_t, AxisEventHandler> axes;
        std::map<uint8_t, ButtonEventHandler> buttons;
        std::map<uint8_t, HatEventHandler> hats;
      };
      std::map<gameinput::InputDevice*, DeviceMappings> mMappings;
      std::vector<vjoypp::OutputDevice*> mToFlush;
  };
} // namespace fredemmott::inputmapping
