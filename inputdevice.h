#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace fredemmott::gameinput {

struct AxisInformation;

struct VIDPID {
  uint16_t vid;
  uint16_t pid;
  bool operator==(const VIDPID& other) const;
};


class InputDevice {
 public:
  InputDevice(IDirectInput8* di, LPCDIDEVICEINSTANCE device);
  ~InputDevice();

  std::string getInstanceName() const;
  std::string getProductName() const;
  std::optional<VIDPID> getVIDPID() const;

  uint32_t getAxisCount();
  std::vector<AxisInformation> getAxisInformation();
  uint32_t getButtonCount();
  uint32_t getHatCount();

  HANDLE getEvent();

  void activate();

  std::vector<uint8_t> getState();
 private:
  struct Impl;
  std::unique_ptr<Impl> p;
  HANDLE mEventHandle = nullptr;
  bool mActivated = false;
  size_t mDataSize = 0;
};
} // namespace fredemmott::gameinput
