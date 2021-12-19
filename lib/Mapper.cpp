/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "Mapper.h"

#include "EventSink.h"
#include "EventSource.h"
#include "InputDevice.h"

namespace fredemmott::inputmapping {

namespace {

HANDLE gExitEvent;
BOOL WINAPI exit_event_handler(DWORD dwCtrlType) {
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
}// namespace

void Mapper::setEventSinks(
  const std::vector<std::shared_ptr<EventSink>>& sinks) {
  mEventSinks = sinks;
}

void Mapper::setEventSources(
  const std::vector<std::shared_ptr<EventSource>>& sources) {
  mEventSources = sources;
}

void Mapper::run() {
  if (mEventSources.empty()) {
    printf(
      "---\n"
      "!!! WARNING !!!\n"
      "No inputs were set. No mapping will be updated unless devices are\n"
      "manually polled.\n"
      "---\n");
  }
  if (mEventSinks.empty()) {
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
  std::vector<HANDLE> fixed_events {gExitEvent};

  std::map<HANDLE, std::shared_ptr<EventSource>> handle_to_source;
  for (const auto& source: mEventSources) {
    auto handle = source->getHandle();
    fixed_events.push_back(handle);
    handle_to_source.insert({handle, source});
  }
  printf("---\nProfile running, hit Ctrl-C to exit and clean up HidHide.\n");
  while (true) {
    auto events = fixed_events;
    for (const auto& [event, _]: mInjected) {
      events.push_back(event);
    }
    const auto res
      = WaitForMultipleObjects(events.size(), events.data(), false, INFINITE);
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

    auto source = handle_to_source.at(event);
    source->poll();
    gActiveInstance = nullptr;

    flush();
  }
}

void Mapper::flush() {
  for (const auto& output: mEventSinks) {
    output->flush();
  }
}

namespace {
typedef std::chrono::
  duration<int64_t, std::ratio_multiply<std::hecto, std::nano>>

    FILETIME_RESOLUTION;
}

void Mapper::inject(
  const std::chrono::steady_clock::duration& delay,
  const std::function<void()>& handler) {
  if (!gActiveInstance) {
    return;
  }
  int64_t target_time;
  GetSystemTimeAsFileTime((FILETIME*)&target_time);
  target_time += std::chrono::duration_cast<FILETIME_RESOLUTION>(delay).count();

  auto timer = CreateWaitableTimer(nullptr, true, nullptr);
  SetWaitableTimer(
    timer, (LARGE_INTEGER*)&target_time, 0, nullptr, nullptr, false);
  gActiveInstance->mInjected.emplace(timer, handler);
}
}// namespace fredemmott::inputmapping
