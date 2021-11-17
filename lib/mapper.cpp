/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "mapper.h"

#include "inputdevice.h"
#include "MappableInput.h"
#include "MappableVJoyOutput.h"
#include "VJoyDevice.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace fredemmott::inputmapping
{

  using fredemmott::gameinput::InputDevice;

  namespace
  {
    struct DeviceOffsets
    {
      const off_t firstAxis;
      const off_t firstHat;
      const off_t firstButton;
      DeviceOffsets(InputDevice *dev) : firstAxis(0),
                                        firstHat(firstAxis + (sizeof(long) * dev->getAxisCount())),
                                        firstButton(firstHat + sizeof(int32_t) * dev->getHatCount())
      {
      }
    };

    struct DeviceState
    {
      const long *const axes;
      const bool *const buttons;
      const int32_t *const hats;

      DeviceState(const uint8_t *data, const DeviceOffsets &offsets) : axes((long *)(data + offsets.firstAxis)),
                                                                       buttons((bool *)(data + offsets.firstButton)),
                                                                       hats((int32_t *)(data + offsets.firstHat))
      {
      }
    };

    HANDLE gExitEvent;
    BOOL WINAPI exit_event_handler(
        DWORD dwCtrlType)
    {
      SetEvent(gExitEvent);
      return true;
    }

    Mapper *gActiveInstance = nullptr;

    struct ActiveInstanceGuard
    {
      ActiveInstanceGuard(Mapper *active)
      {
        gActiveInstance = active;
      }
      ~ActiveInstanceGuard()
      {
        gActiveInstance = nullptr;
      }
    };
  } // namespace

  void Mapper::setDevices(
      const std::vector<MappableInput> &inputs,
      const std::vector<std::shared_ptr<OutputDevice>> &outputs)
  {
    mInputs = inputs;
    mOutputs = outputs;
  }

  void Mapper::passthrough(MappableInput &s, const MappableVJoyOutput &t)
  {
    s.XAxis.bind(t.XAxis);
    s.YAxis.bind(t.YAxis);
    s.ZAxis.bind(t.ZAxis);
    s.RXAxis.bind(t.RXAxis);
    s.RYAxis.bind(t.RYAxis);
    s.RZAxis.bind(t.RZAxis);
    s.Slider.bind(t.RZAxis);
    for (int i = 1; i <= s.getButtonCount(); ++i)
    {
      s.button(i).bind(t.button(i));
    }
    for (int i = 1; i <= s.getHatCount(); ++i)
    {
      s.hat(i).bind(t.hat(i));
    }
  }

  void Mapper::run()
  {
    if (mInputs.empty())
    {
      printf(
          "---\n"
          "!!! WARNING !!!\n"
          "No inputs were set. No mapping will be updated unless devices are\n"
          "manually polled.\n"
          "---\n");
    }
    if (mOutputs.empty())
    {
      printf(
          "---\n"
          "!!! WARNING !!!\n"
          "No outputs were set. VJoy/ViGEm outputs will not be updated\n"
          "unless manually flushed.\n"
          "---\n");
    }
    printf("Launching profile...\n");
    // We want to cleanly exit so that destructors are called - in particular,
    // we want to reset the HidHide configuration.
    gExitEvent = CreateEvent(nullptr, false, false, nullptr);
    SetConsoleCtrlHandler(&exit_event_handler, true);
    std::vector<HANDLE> fixed_events{gExitEvent};

    std::map<HANDLE, MappableInput> devices;
    for (const auto &input : mInputs)
    {
      auto event = input.getDevice()->getEvent();
      fixed_events.push_back(event);
      devices.insert({event, input});
    }
    printf("---\nProfile running, hit Ctrl-C to exit and clean up HidHide.\n");
    while (true)
    {
      auto events = fixed_events;
      for (const auto &[event, _] : mInjected)
      {
        events.push_back(event);
      }
      const auto res = WaitForMultipleObjects(
          events.size(),
          events.data(),
          false,
          INFINITE);
      auto event = events[res - WAIT_OBJECT_0];
      if (event == gExitEvent)
      {
        SetConsoleCtrlHandler(nullptr, false);
        CloseHandle(gExitEvent);
        printf("Exiting.\n---\n");
        break;
      }

      ActiveInstanceGuard aig(this);

      auto injected = mInjected.find(event);
      if (injected != mInjected.end())
      {
        injected->second();
        mInjected.erase(event);
        CloseHandle(event);
        flush();
        continue;
      }

      auto device = devices.at(event);
      device.poll();
      gActiveInstance = nullptr;

      flush();
    }
  }

  void Mapper::flush()
  {
    for (const auto &output : mOutputs)
    {
      output->flush();
    }
  }

  namespace
  {
    typedef std::chrono::duration<int64_t, std::ratio_multiply<std::hecto, std::nano>>

        FILETIME_RESOLUTION;
  }

  void Mapper::inject(
      const std::chrono::steady_clock::duration &delay,
      const std::function<void()> &handler)
  {
    if (!gActiveInstance)
    {
      return;
    }
    int64_t target_time;
    GetSystemTimeAsFileTime((FILETIME *)&target_time);
    target_time += std::chrono::duration_cast<FILETIME_RESOLUTION>(delay).count();

    auto timer = CreateWaitableTimer(nullptr, true, nullptr);
    SetWaitableTimer(timer, (LARGE_INTEGER *)&target_time, 0, nullptr, nullptr, false);
    gActiveInstance->mInjected.emplace(timer, handler);
  }
} // namespace fredemmott::inputmapping
