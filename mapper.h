#pragma once

#include "actionsapi.h"
#include "actionortarget.h"

#include <map>

namespace fredemmott::gameinput {
  class InputDevice;
}

namespace fredemmott::inputmapping {
  struct MappableInput;
  struct MappableOutput;

  class Mapper {
    public:
      void run();

      void passthrough(const MappableInput&, const MappableOutput&);

      void map(const AxisSource& source, const AxisActionOrTarget& handler);
      void map(const ButtonSource& source, const ButtonActionOrTarget& handler);
      void map(const HatSource& source, const HatActionOrTarget& handler);

      struct ButtonMapping {
        ButtonSource source;
        ButtonActionOrTarget action;
      };
      struct HatMapping {
        HatSource source;
        HatActionOrTarget action;
      };
      struct AxisMapping {
        AxisSource source;
        AxisActionOrTarget action;
      };
      void map(const std::initializer_list<AxisMapping>& rules);
      void map(const std::initializer_list<ButtonMapping>& rules);
      void map(const std::initializer_list<HatMapping>& rules);

    private:
      struct DeviceMappings {
        std::map<uint8_t, AxisAction*> axes;
        std::map<uint8_t, ButtonAction*> buttons;
        std::map<uint8_t, HatAction*> hats;
      };
      std::map<gameinput::InputDevice*, DeviceMappings> mMappings;
  };
} // namespace fredemmott::inputmapping
