#define NOMINMAX

#include <limits>
#include <map>
#include <set>

#include <cassert>
#include <cstdio>

#include "axistypes.h"
#include "inputdevice.h"
#include "inputdevicecollection.h"
#include "vjoypp.h"

#include <winreg.h>
#pragma comment(lib, "Advapi32.lib")

namespace vjoypp = fredemmott::vjoypp;
using namespace fredemmott::gameinput;

void dump_state(InputDevice* device) {
  printf("State:\n");
  const auto buf = device->getState();
  const BYTE* data = buf.data();
  off_t offset = 0;
  for (int i = 1; i <= device->getAxisCount(); ++i) {
    std::string type;
    switch(device->getAxisInformation()[i - 1].type) {
      case AxisType::X:
        type = "X";
        break;
      case AxisType::Y:
        type = "Y";
        break;
      case AxisType::Z:
        type = "Z";
        break;
      case AxisType::RX:
        type = "Rx";
        break;
      case AxisType::RY:
        type = "Ry";
        break;
      case AxisType::RZ:
        type = "Rz";
        break;
      case AxisType::SLIDER:
        type = "Slider";
        break;
    }
    printf("  Axis %d: %ld\t(%s)\n", i, *(long*)&buf[offset], type.c_str());
    offset += sizeof(long);
  }
  for (int i = 1; i <= device->getHatCount(); ++i) {
    printf("  Hat %d: 0x%0.2x\n", i, *(int32_t*)&buf[offset]);
    offset += sizeof(int32_t);
  }
  printf("  Buttons:");
  for (int i = 1; i <= device->getButtonCount(); ++i) {
    if (buf[offset]) {
      printf(" %d", i);
    }
    offset += 1;
  }
  printf("\n");
}

template<typename TValue>
class Action {
 public:
   virtual std::set<vjoypp::OutputDevice*> getAffectedDevices() = 0;
   virtual void map(TValue value) = 0;
};

class AxisAction: public Action<long> {};
class ButtonAction : public Action<bool> {};
class HatAction : public Action<int16_t> {};

template<typename TDevice>
struct MappableButton {
  TDevice* device;
  uint8_t button;
};
typedef MappableButton<InputDevice> ButtonSource;
typedef MappableButton<vjoypp::OutputDevice> ButtonTarget;

template<typename TDevice>
struct MappableHat {
  TDevice* device;
  uint8_t hat;
};
typedef MappableHat<InputDevice> HatSource;
typedef MappableHat<vjoypp::OutputDevice> HatTarget;


struct AxisSource {
  InputDevice* device;
  uint8_t axis;
};

struct AxisTarget {
  vjoypp::OutputDevice* device;
  const char* label;
  vjoypp::OutputDevice* (vjoypp::OutputDevice::*setter)(long);
};


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

class AxisToButtons : public AxisAction {
 public:
  struct Range {
    uint8_t minPercent;
    uint8_t maxPercent;
    ButtonTarget target;
  };

  AxisToButtons(std::initializer_list<Range> ranges) {
    const long max = std::numeric_limits<uint16_t>::max();
    for (const auto& range: ranges) {
      mRanges.push_back({(range.minPercent * max) / 100, (range.maxPercent * max) / 100, range.target});
    }
  }

  void map(long value) {
    for (const auto& range: mRanges) {
      bool active = range.min <= value && range.max >= value;
      range.target.device->setButton(range.target.button, active);
    }
  }

  std::set<vjoypp::OutputDevice*> getAffectedDevices() {
    std::set<vjoypp::OutputDevice*> out;
    for (const auto& range: mRanges) {
      out.emplace(range.target.device);
    }
    return out;
  }
 private:
  struct RawRange {
    long min;
    long max;
    ButtonTarget target;
  };
  std::vector<RawRange> mRanges;
};

namespace {
  struct DeviceOffsets {
    const off_t firstAxis;
    const off_t firstHat;
    const off_t firstButton;
  };
}

class AxisPassthrough : public AxisAction {
  public:
  AxisPassthrough(const AxisTarget& target): AxisAction(), mTarget(target) {
  }

  void map(long value) {
    (mTarget.device->*(mTarget.setter))(value);
  }

  std::set<vjoypp::OutputDevice*> getAffectedDevices() {
    return { mTarget.device };
  }
  private:
    AxisTarget mTarget;
};

class ButtonPassthrough : public ButtonAction {
  public:
  ButtonPassthrough(const ButtonTarget& target): ButtonAction(), mTarget(target) {
  }

  void map(bool value) {
    mTarget.device->setButton(mTarget.button, value);
  }

  std::set<vjoypp::OutputDevice*> getAffectedDevices() {
    return { mTarget.device };
  }
  private:
    ButtonTarget mTarget;
};

class HatPassthrough : public HatAction {
  public:
  HatPassthrough(const HatTarget& target): HatAction(), mTarget(target) {
  }

  void map(int16_t value) {
    mTarget.device->setHat(mTarget.hat, value);
  }

  std::set<vjoypp::OutputDevice*> getAffectedDevices() {
    return { mTarget.device };
  }
  private:
    HatTarget mTarget;
};

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
     std::vector<HANDLE> events;
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
  const char* prefix = "SYSTEM\\CurrentControlSet\\Services\\HidGuardian\\Parameters\\Whitelist";
  char buf[256];
  snprintf(buf, sizeof(buf), "%s\\%d", prefix, GetCurrentProcessId());
  HKEY regkey;
  auto ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, buf, NULL, NULL, REG_OPTION_VOLATILE,
      KEY_ALL_ACCESS, 
      nullptr, &regkey, nullptr
      );
  if (ret == ERROR_ACCESS_DENIED) {
    printf(
      "Access denied to HidGuardian whitelist. If you're Using HidGuardian,\n"
      "Open RegEdit as administrator, create the following key, and give\n"
      "your non-administrator user full permissions to it:\n"
      "  HKEY_LOCAL_MACHINE\\%s\n---\n",
      prefix
    );
  }
  RegCloseKey(regkey);

  vjoypp::init();
  InputDeviceCollection devices;
  auto device = devices.get({0x044f, 0x0404}); // warthog throttle
  //auto device = devices.get({0x3344, 0x40cc}); // warbrd right
  if (!device) {
    printf("Failed to find device:'(\n");
    return 0;
  }
  printf("Found device: %s\n", device->getProductName().c_str());

  MappableInputDevice s(device);
  MappableOutputDevice t(new vjoypp::OutputDevice(1));

  Mapper mapper;
  mapper.passthrough(s, t);
  const auto first = device->getButtonCount() + 1;
  mapper.map(
    { // axes
      {
        s.XAxis,
        AxisToButtons {
          { 0, 0, t.button(first) },
          { 100, 100, t.button(first + 1) }
        },
      },
      {
        s.YAxis,
        AxisToButtons {
          { 0, 0, t.button(first + 2) },
          { 100, 100, t.button(first + 3) }
        },
      },
    },
    {}, // hats
    {} // buttons
  );
  mapper.run();
  return 0;
}
