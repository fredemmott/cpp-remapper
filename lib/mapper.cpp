/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
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
      DeviceOffsets(InputDevice* dev) :
        firstAxis(0),
        firstHat(firstAxis + (sizeof(long) * dev->getAxisCount())),
        firstButton(firstHat + sizeof(int32_t) * dev->getHatCount()) {
      }
    };

    struct DeviceState {
      const long* const axes;
      const bool* const buttons;
      const int32_t* const hats;

      DeviceState(const uint8_t* data, const DeviceOffsets& offsets):
        axes((long*) (data + offsets.firstAxis)),
        buttons((bool*) (data + offsets.firstButton)),
        hats((int32_t*) (data + offsets.firstHat)) {
      }
    };

    HANDLE gExitEvent;
    BOOL WINAPI exit_event_handler(
        DWORD dwCtrlType
        ) {
      SetEvent(gExitEvent);
      return true;
    }

    Mapper* gActiveInstance = nullptr;

    struct ActiveInstanceGuard {
      ActiveInstanceGuard(Mapper* active) {
        gActiveInstance = active;
      }
      ~ActiveInstanceGuard() {
        gActiveInstance = nullptr;
      }
    };
  } // namespace

  void Mapper::setOutputs(const std::vector<MappableOutput>& outputs) {
    mToFlush.clear();
    for (const auto& output: outputs) {
      mToFlush.push_back(output.getDevice());
    }
  }

  namespace {
    template<typename A, typename B, typename C>
    void map_impl(A& collection, const B& id, const C& handler) {
      if (handler) {
        collection.insert_or_assign(id - 1, handler);
        return;
      }
      collection.erase(id - 1);
    }
  } // namespace

  void Mapper::map(const AxisSource& source, const AxisEventHandler& handler) {
    if (!(source.device && source.axis)) {
      return;
    }
    map_impl(mMappings[source.device].axes, source.axis, handler);
  }

  void Mapper::map(const ButtonSource& source, const ButtonEventHandler& handler) {
    if (!(source.device && source.button)) {
      return;
    }
    map_impl(mMappings[source.device].buttons, source.button, handler);
  }

  void Mapper::map(const HatSource& source, const HatEventHandler& handler) {
    if (!(source.device && source.hat)) {
      return;
    }
    map_impl(mMappings[source.device].hats, source.hat, handler);
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
    for (int i = 1; i <= s.ButtonCount; ++i) {
      map(s.button(i), t.button(i));
    }
    for (int i = 1; i <= s.HatCount; ++i) {
      map(s.hat(i), t.hat(i));
    }
  }

  void Mapper::run() {
    if (mToFlush.empty()) {
      printf(
        "---\n"
        "!!! WARNING !!!\n"
        "Mapper::setOutputs() was not called, or was passed an "
        "empty list. VJoy outputs will be stale unless manually flushed.\n"
        "---\n"
      );
    }
    printf("Launching profile...\n");
    // We want to cleanly exit so that destructors are called - in particular,
    // we want to reset the HidGuardian configuration.
    gExitEvent = CreateEvent(nullptr, false, false, nullptr);
    SetConsoleCtrlHandler(&exit_event_handler, true);
    std::vector<HANDLE> fixed_events { gExitEvent };

    std::map<HANDLE, InputDevice*> devices;
    std::map<InputDevice*, std::vector<uint8_t>> states;
    std::map<InputDevice*, DeviceOffsets> offsets;
    for (const auto& [device, mapping]: mMappings) {
      auto event = device->getEvent();
      fixed_events.push_back(event);
      devices[event] = device;

      DeviceOffsets offs(device);
      offsets.emplace(device, offs);

      auto state_buf = device->getState();
      states.emplace(device, state_buf);
      const DeviceState state(state_buf.data(), offs);

      for (const auto& [i, handler] : mapping.axes) {
        handler->map(state.axes[i]);
      }
      for (const auto& [i, handler] : mapping.buttons) {
        handler->map(state.buttons[i]);
      }
      for (const auto& [i, handler] : mapping.hats) {
        handler->map(state.hats[i]);
      }
    }
    printf("---\nProfile running, hit Ctrl-C to exit and clean up HidGuardian.\n");
    while (true) {
      auto events = fixed_events;
      for (const auto& [event, _]: mInjected) {
        events.push_back(event);
      }
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

      ActiveInstanceGuard aig(this);

      auto injected = mInjected.find(event);
      if (injected != mInjected.end()) {
        injected->second();
        mInjected.erase(event);
        CloseHandle(event);
        flush();
        continue;
      }

      auto device = devices.at(event);
      const auto& offs = offsets.at(device);
      const auto& old_buf = states.at(device);
      const DeviceState old_state(old_buf.data(), offs);
      const auto new_buf = device->getState();
      const DeviceState new_state(new_buf.data(), offs);

      const auto& mappings = mMappings.at(device);
      for (const auto [i, action]: mappings.axes) {
        if (old_state.axes[i] == new_state.axes[i]) {
          continue;
        }
        action->map(new_state.axes[i]);
      }
      for (const auto& [i, action]: mappings.hats) {
        if (old_state.hats[i] == new_state.hats[i]) {
          continue;
        }
        action->map(new_state.hats[i]);
      }
      for (const auto& [i, action]: mappings.buttons) {
        if (old_state.buttons[i] == new_state.buttons[i]) {
          continue;
        }
        action->map(new_state.buttons[i]);
      }
      gActiveInstance = nullptr;

      states[device] = new_buf;
      flush();
    }
  }

  void Mapper::flush() {
    for (const auto& device: mToFlush) {
      device->send();
    }
  }

  namespace {
    typedef std::chrono::duration<int64_t, std::ratio_multiply<std::hecto, std::nano>>

    FILETIME_RESOLUTION;
  }

  void Mapper::inject(
    const std::chrono::steady_clock::duration& delay,
    const std::function<void()>& handler
  ) {
    if (!gActiveInstance) {
      return;
    }
    int64_t target_time;
    GetSystemTimeAsFileTime((FILETIME*) &target_time);
    target_time += std::chrono::duration_cast<FILETIME_RESOLUTION>(delay).count();

    auto timer = CreateWaitableTimer(nullptr, true, nullptr);
    SetWaitableTimer(timer, (LARGE_INTEGER*) &target_time, 0, nullptr, nullptr, false);
    gActiveInstance->mInjected.emplace(timer, handler);
  }
} // namespace fredemmott::inputmapping

// --- Test ---

using namespace fredemmott::inputmapping;
using namespace fredemmott::inputmapping::actions;

namespace {

class InvertButton final : public ButtonAction {
private:
  ButtonEventHandler mNext;
public:

  InvertButton(const ButtonEventHandler& next) : mNext(next) {}
  ~InvertButton() {}

  void map(bool value) {
    mNext->map(!value);
  }
};

void static_test() {
  Mapper m;
  MappableInput i(nullptr);
  MappableOutput o(1);

  // Basic passthrough
  //   Impl: (Axis|Button|Hat)Target -> VJoy(Axis|Button|Hat)
  m.map(i.XAxis, o.XAxis);
  m.map(i.Button1, o.Button1);
  m.map(i.Hat1, o.Hat1);

  // 1:n
  //   Impl: std::initializer_list<EventHandler> -> ComboTarget
  m.map(i.XAxis, {o.XAxis, o.YAxis});
  m.map(i.Button1, {o.Button1, o.Button2});
  m.map(i.Hat1, {o.Hat1, o.Hat2});

  // Lambdas
  //   Impl: T -> std::function<void(TValue)> -> FunctionAction
  m.map(i.XAxis, [=](long v) { o.XAxis.set(v); });
  m.map(i.Button1, [=](bool v) { o.Button1.set(v); });
  m.map(i.Hat1, [=](int16_t v) { o.Hat1.set(v); });

  // Here onwards, assuming that axis/button/hats are abstracted
  // adequately (tested above), so we don't need to test them all

  // Explicit actions
  m.map(i.Button1, InvertButton { o.Button1 });

  // Chained actions
  m.map(i.Button1, InvertButton { InvertButton { o.Button1 } });

  // Chain to lambda
  m.map(i.Button1, InvertButton { [](bool){} });

  // 1:n lambdas
  m.map(
    i.Button1,
    {
      [=](bool v) { o.Button1.set(v); },
      [=](bool v) { o.Button2.set(v); },
    }
  );

  // Multiple bindings at the same time
  m.map({
    { i.Button1, o.Button1 },
    { i.Button2, { o.Button2, o.Button3 } },
    { i.Button3, [](bool){} },
    { i.Button3, {[](bool){}, [](bool){} } },
  });

  // Remove an existing binding
  m.map(i.Button1, nullptr);
  m.map({{i.Button1, nullptr}});
  m.map(i.Slider, nullptr);
  m.map({{i.Slider, nullptr}});
}

} // namespace ( tests)
