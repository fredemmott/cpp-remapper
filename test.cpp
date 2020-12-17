#define NOMINMAX

#include <limits>
#include <map>
#include <set>

#include <cassert>
#include <cstdio>

#include "axistypes.h"
#include "devicedb.h"
#include "hidguardian.h"
#include "inputdevice.h"
#include "inputdevicecollection.h"
#include "vjoypp.h"

#include "axistobuttons.h"
#include "passthroughs.h"

namespace vjoypp = fredemmott::vjoypp;
using namespace fredemmott::gameinput;
using namespace fredemmott::inputmapping;

template<typename TDevice>
struct MappableDevice {
 MappableDevice(TDevice* dev): device(dev) {}
 MappableButton<TDevice> button(uint8_t number) {
   return {device, number};
 }
 MappableHat<TDevice> hat(uint8_t number) {
   return {device, number};
 }

 TDevice* getDevice() const {
   return device;
 }
 protected:
  TDevice* device;
};

struct MappableOutputDevice : public MappableDevice<vjoypp::OutputDevice> {
  MappableOutputDevice(vjoypp::OutputDevice* dev):
    MappableDevice(dev),
    XAxis { dev, "X", &vjoypp::OutputDevice::setXAxis },
    YAxis { dev, "Y", &vjoypp::OutputDevice::setYAxis },
    ZAxis { dev, "Z", &vjoypp::OutputDevice::setZAxis },
    RXAxis { dev, "RX", &vjoypp::OutputDevice::setRXAxis },
    RYAxis { dev, "RY", &vjoypp::OutputDevice::setRYAxis },
    RZAxis { dev, "RZ", &vjoypp::OutputDevice::setRZAxis },
    Slider { dev, "Slider", &vjoypp::OutputDevice::setSlider},
    Dial { dev, "Dial", &vjoypp::OutputDevice::setDial}
  {
  }

  const AxisTarget XAxis;
  const AxisTarget YAxis;
  const AxisTarget ZAxis;
  const AxisTarget RXAxis;
  const AxisTarget RYAxis;
  const AxisTarget RZAxis;
  const AxisTarget Slider;
  const AxisTarget Dial;
};

struct MappableInputDevice : public MappableDevice<InputDevice> {
  MappableInputDevice(InputDevice* dev):
    MappableDevice(dev),
    XAxis(findAxis(AxisType::X)),
    YAxis(findAxis(AxisType::Y)),
    ZAxis(findAxis(AxisType::Z)),
    RXAxis(findAxis(AxisType::RX)),
    RYAxis(findAxis(AxisType::RY)),
    RZAxis(findAxis(AxisType::RZ)),
    Slider(findAxis(AxisType::SLIDER))
  {
  }
  const AxisSource XAxis;
  const AxisSource YAxis;
  const AxisSource ZAxis;
  const AxisSource RXAxis;
  const AxisSource RYAxis;
  const AxisSource RZAxis;
  const AxisSource Slider;

  AxisSource axis(uint8_t id) {
    return { device, id };
  }
 private:
  AxisSource findAxis(AxisType t) {
    const auto info = device->getAxisInformation();
    for (uint8_t i = 0; i < info.size(); ++i) {
      if (info[i].type == t) {
        return {device, ++i };
      }
    }
    return {device, 0};
  }
};

namespace {
  struct DeviceOffsets {
    const off_t firstAxis;
    const off_t firstHat;
    const off_t firstButton;
  };
}


template<typename Target, typename Action, typename Passthrough>
class ActionWrapper {
public:
 ActionWrapper(const Target& target) {
   mAction = new Passthrough { target } ;
 }
 template<typename T>
 ActionWrapper(const T& action) {
   mAction = new T(action);
 }
 Action* getAction() const {
   return mAction;
 }
private:
 Action* mAction;
};
class AxisActionWrapper : public ActionWrapper<AxisTarget, AxisAction, AxisPassthrough> {};
class ButtonActionWrapper : public ActionWrapper<ButtonTarget, ButtonAction, ButtonPassthrough> {};
class HatActionWrapper : public ActionWrapper<HatTarget, HatAction, HatPassthrough> {};

namespace {
  HANDLE gExitEvent;
  BOOL WINAPI exit_event_handler(
    DWORD dwCtrlType
  ) {
    SetEvent(gExitEvent);
    return true;
  }
}

class Mapper {
 public:
   struct AxisMapping {
     AxisSource source;
     AxisActionWrapper action;
   };
   struct ButtonMapping {
     ButtonSource source;
     ButtonActionWrapper action;
    };
   struct HatMapping {
     HatSource source;
     HatActionWrapper action;
   };
   void map(
     const std::initializer_list<AxisMapping>& axes,
     const std::initializer_list<HatMapping>& hats,
     const std::initializer_list<ButtonMapping>& buttons
  ) {
     for (const auto& mapping: axes) {
       mMappings[mapping.source.device].axes[mapping.source.axis] = mapping.action.getAction();
     }
     for (const auto& mapping: hats) {
       mMappings[mapping.source.device].hats[mapping.source.hat] = mapping.action.getAction();
     }
     for (const auto& mapping: buttons) {
       mMappings[mapping.source.device].buttons[mapping.source.button] = mapping.action.getAction();
     }
   }

   void passthrough(MappableInputDevice& s, MappableOutputDevice& t) {
     map({
        { s.XAxis, t.XAxis },
        { s.YAxis, t.YAxis },
        { s.ZAxis, t.ZAxis },
        { s.RXAxis, t.RXAxis },
        { s.RYAxis, t.RYAxis },
        { s.RZAxis, t.RZAxis },
        { s.Slider, t.Slider },
        // TODO: dial (a.k.a. slider 2)
     }, {}, {});
     auto sd = s.getDevice();
     const auto hats = sd->getHatCount();
     for (int i = 1; i <= hats; ++i) {
       mMappings[sd].hats[i] = new HatPassthrough(t.hat(i));
     }
     const auto buttons = sd->getButtonCount();
     for (int i = 1; i <= buttons; ++i) {
       mMappings[sd].buttons[i] = new ButtonPassthrough(t.button(i));
     }
   }

   void run() {
     gExitEvent = CreateEvent(nullptr, false, false, nullptr);
     SetConsoleCtrlHandler(&exit_event_handler, true);
     std::vector<HANDLE> events { gExitEvent };
     std::map<HANDLE, InputDevice*> devices;
     std::map<InputDevice*, std::vector<uint8_t>> states;
     std::map<InputDevice*, DeviceOffsets> offsets;
     for (const auto& [device, _]: mMappings) {
       auto event = device->getEvent();
       events.push_back(event);
       devices[event] = device;
       states[device] = device->getState();
       const off_t hat_offset = sizeof(long) * device->getAxisCount();
       const off_t button_offset = hat_offset + (sizeof(int32_t) * device->getHatCount());
       offsets.emplace(device, DeviceOffsets { 0, hat_offset, button_offset });
     }
     while (true) {
       const auto res = WaitForMultipleObjects(
         events.size(),
         events.data(),
         false,
         INFINITE
       );
       auto event = events[res - WAIT_OBJECT_0];
       if (event == gExitEvent) {
         SetConsoleCtrlHandler(nullptr, false);
         CloseHandle(gExitEvent);
         break;
       }

       auto device = devices[event];
       const auto old_state = states[device];
       const auto new_state = device->getState();
       states[device] = new_state;
       const auto& device_offsets = offsets.at(device);
       auto old_axis = reinterpret_cast<const long*>(&old_state.data()[device_offsets.firstAxis]);
       auto old_hat = reinterpret_cast<const int16_t*>(&old_state.data()[device_offsets.firstHat]);
       auto old_button = reinterpret_cast<const uint8_t*>(&old_state.data()[device_offsets.firstButton]);
       auto new_axis = reinterpret_cast<const long*>(&new_state.data()[device_offsets.firstAxis]);
       auto new_hat = reinterpret_cast<const int16_t*>(&new_state.data()[device_offsets.firstHat]);
       auto new_button = reinterpret_cast<const uint8_t*>(&new_state.data()[device_offsets.firstButton]);

       const auto &mappings = mMappings[device];
       std::set<vjoypp::OutputDevice*> affected;

       for (const auto& [axis_id, action]: mappings.axes) {
         const auto idx = axis_id - 1;
         if (old_axis[idx] == new_axis[idx]) {
           continue;
          }
         action->map(new_axis[idx]);
         affected.merge(action->getAffectedDevices());
       }
       for (const auto& [hat_id, action]: mappings.hats) {
         const auto idx = hat_id - 1;
         if (old_hat[idx] == new_hat[idx]) {
           continue;
          }
         action->map(new_hat[idx]);
         affected.merge(action->getAffectedDevices());
       }
       for (const auto& [button_id, action]: mappings.buttons) {
         const auto idx = button_id - 1;
         if (old_button[idx] == new_button[idx]) {
           continue;
          }
         action->map(new_button[idx]);
         affected.merge(action->getAffectedDevices());
       }

       for (const auto& device: affected) {
         device->send();
       }
     }
   }
  private:
   struct DeviceMappings {
     std::map<uint8_t, AxisAction*> axes;
     std::map<uint8_t, ButtonAction*> buttons;
     std::map<uint8_t, HatAction*> hats;
   };
   std::map<InputDevice*, DeviceMappings> mMappings;
};

int main() {
  HidGuardian hid({TM_WARTHOG_THROTTLE, VPC_RIGHT_WARBRD});
  vjoypp::init();
  InputDeviceCollection devices;

  MappableInputDevice throttle(devices.get(TM_WARTHOG_THROTTLE));
  MappableInputDevice stick(devices.get(VPC_RIGHT_WARBRD));
  MappableOutputDevice vj1(new vjoypp::OutputDevice(1));
  MappableOutputDevice vj2(new vjoypp::OutputDevice(2));

  Mapper mapper;
  mapper.passthrough(throttle, vj1);
  mapper.passthrough(stick, vj2);
  const auto tbc = throttle.getDevice()->getButtonCount();
  const auto sbc = stick.getDevice()->getButtonCount();
  mapper.map(
    { // axes
      {
        throttle.XAxis,
        AxisToButtons {
          { 0, 0, vj1.button(tbc + 1) },
          { 100, 100, vj1.button(tbc + 2) }
        },
      }, {
        throttle.YAxis,
        AxisToButtons {
          { 0, 0, vj1.button(tbc + 3) },
          { 100, 100, vj1.button(tbc + 4) }
        },
      }, {
        stick.RXAxis,
        AxisToButtons {
          { 0, 0, vj2.button(sbc + 1) },
          { 100, 100, vj2.button(sbc + 2) }
        },
      }, {
        stick.RYAxis,
        AxisToButtons {
          { 0, 0, vj2.button(sbc + 3) },
          { 100, 100, vj2.button(sbc + 4) }
        },
      },
    },
    {}, // hats
    {} // buttons
  );
  printf("Launching profile...\n");
  mapper.run();
  printf("Exiting cleanly.\n");
  return 0;
}
