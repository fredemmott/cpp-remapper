#include "mapper.h"

#include "inputdevice.h"
#include "vjoypp.h"
#include "mappabledevices.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace fredemmott::inputmapping {

  using fredemmott::gameinput::InputDevice;

  namespace {
    struct DeviceOffsets {
      const off_t firstAxis;
      const off_t firstHat;
      const off_t firstButton;
    };

    HANDLE gExitEvent;
    BOOL WINAPI exit_event_handler(
        DWORD dwCtrlType
        ) {
      SetEvent(gExitEvent);
      return true;
    }
  } // namespace

  void Mapper::map(const AxisSource& source, const AxisActionOrTarget& handler) {
    if (source.device && source.axis && handler) {
      mMappings[source.device].axes.insert_or_assign(
          source.axis,
          handler
          );
    }
  }

  void Mapper::map(const ButtonSource& source, const ButtonActionOrTarget& handler) {
    if (source.device && source.button && handler) {
      mMappings[source.device].buttons.insert_or_assign(
          source.button,
          handler
          );
    }
  }

  void Mapper::map(const HatSource& source, const HatActionOrTarget& handler) {
    if (source.device && source.hat && handler) {
      mMappings[source.device].hats.insert_or_assign(
          source.hat,
          handler
          );
    }
  }

  void Mapper::map(const std::initializer_list<AxisMapping>& rules) {
    for (const auto& rule: rules) {
      map(rule.source, rule.action);
    }
  }

  void Mapper::map(const std::initializer_list<ButtonMapping>& rules) {
    for (const auto& rule: rules) {
      map(rule.source, rule.action);
    }
  }

  void Mapper::map(const std::initializer_list<HatMapping>& rules) {
    for (const auto& rule: rules) {
      map(rule.source, rule.action);
    }
  }

  void Mapper::passthrough(const MappableInput& s, const MappableOutput& t) {
    map({
        { s.XAxis, t.XAxis },
        { s.YAxis, t.YAxis },
        { s.ZAxis, t.ZAxis },
        { s.RXAxis, t.RXAxis },
        { s.RYAxis, t.RYAxis },
        { s.RZAxis, t.RZAxis },
        { s.Slider, t.Slider },
        // TODO: dial (a.k.a. slider 2)
        });
    auto sd = s.getDevice();
    const auto hats = sd->getHatCount();
    for (int i = 1; i <= hats; ++i) {
      mMappings[sd].hats[i] = new VJoyHat(t.hat(i));
    }
    const auto buttons = sd->getButtonCount();
    for (int i = 1; i <= buttons; ++i) {
      mMappings[sd].buttons[i] = new VJoyButton(t.button(i));
    }
  }

  void Mapper::run() {
    printf("Launching profile...\n");
    // We want to cleanly exit so that destructors are called - in particular,
    // we want to reset the HidGuardian configuration.
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
    printf("---\nProfile running, hit Ctrl-C to exit and clean up HidGuardian.\n");
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
        printf("Exiting.\n---\n");
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

      for (const auto [axis_id, action]: mappings.axes) {
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
} // namespace fredemmott::inputmapping
