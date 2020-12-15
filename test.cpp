#define NOMINMAX
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "User32.lib")

#include <limits>
#include <optional>
#include <string>
#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdio>

enum class AxisType {
  X,
  Y,
  Z,
  RX,
  RY,
  RZ,
  SLIDER,
};

struct AxisInformation {
  AxisType type;
  std::string name;
};

struct VIDPID {
  uint16_t vid;
  uint16_t pid;
  bool operator==(const VIDPID& other) {
    return vid == other.vid && pid == other.pid;
  }
};

class InputDevice {
 public:
  InputDevice(IDirectInput8* di, LPCDIDEVICEINSTANCE device): mDI(di), mDevice(*device) {
  }

  std::string getInstanceName() const {
    return mDevice.tszInstanceName;
  }

  std::string getProductName() const {
    return mDevice.tszProductName;
  }

  std::optional<VIDPID> getVIDPID() const {
    const auto VID_PID_MAGIC = "\0\0PIDVID";
    const auto& guid = mDevice.guidProduct;
    if (*(uint64_t*) guid.Data4 == *(uint64_t*)(VID_PID_MAGIC)) {
      const uint16_t vid = guid.Data1 & 0xffff;
      const uint16_t pid = guid.Data1 >> 16;
      return {{vid, pid}};
    }
    return {};
  }

  uint32_t getAxisCount() {
    enumerateControls();
    return mAxes.size();
  }

  std::vector<AxisInformation> getAxesInformation() {
    return mAxes;
  }

  uint32_t getButtonCount() {
    enumerateControls();
    return mButtons;
  }

  uint32_t getHatCount() {
    enumerateControls();
    return mHats;
  }

  HANDLE getEvent() {
    return mEventHandle;
  }

  void activate() {
    if (mActivated) {
      return;
    }
    const DWORD count = getAxisCount() + getHatCount() + getButtonCount();
    LPDIOBJECTDATAFORMAT df = new DIOBJECTDATAFORMAT[count];
    off_t offset = 0;
    off_t i = 0;
    for (const auto& axis: mAxes) {
      df[i++] = {
        NULL,
        (DWORD) offset,
        DIDFT_AXIS | DIDFT_ANYINSTANCE,
        NULL,
      };
      offset += sizeof(LONG);
    }
    for (size_t j = 0; j < mHats; j++) {
      df[i++] = {
        NULL,
        (DWORD) offset,
        DIDFT_POV | DIDFT_ANYINSTANCE,
        NULL,
      };
      offset += 1;
    }
    for (size_t j = 0; j < mButtons; j++) {
      df[i++] = {
        NULL,
        (DWORD) offset,
        DIDFT_BUTTON | DIDFT_ANYINSTANCE,
        NULL,
      };
      offset += 1;
    }
    mDataSize = offset + (offset % 4 == 0 ? 0 : 4 - (offset % 4));
    DIDATAFORMAT data {
      sizeof(DIDATAFORMAT),
      sizeof(DIOBJECTDATAFORMAT),
      DIDF_ABSAXIS,
      (DWORD) mDataSize,
      (DWORD) i,
      df,
    };

    mDIDevice->SetCooperativeLevel(NULL, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
    mDIDevice->SetDataFormat(&data);
    delete[] df;

    mEventHandle = CreateEvent(nullptr, false, false, nullptr);
    mDIDevice->SetEventNotification(mEventHandle);
    mDIDevice->Acquire();
  }

  std::vector<BYTE> getState() {
    mDIDevice->Poll();
    std::vector<BYTE> buf(mDataSize);
    mDIDevice->GetDeviceState(mDataSize, buf.data());
    return buf;
  }

 private:
  IDirectInput8* mDI;
  DIDEVICEINSTANCE mDevice;
  LPDIRECTINPUTDEVICE8 mDIDevice = nullptr;
  HANDLE mEventHandle = nullptr;
  bool mActivated = false;
  bool mEnumerated = false;
  size_t mDataSize = 0;
  uint32_t mButtons = 0;
  uint32_t mHats = 0;
  std::vector<AxisInformation> mAxes;

  LPDIRECTINPUTDEVICE8 getDIDevice() {
    if (mDIDevice) {
      return mDIDevice;
    }
    mDI->CreateDevice(mDevice.guidInstance, &mDIDevice, nullptr);
    return mDIDevice;
  }

  void enumerateControls() {
    if (mEnumerated) {
      return;
    }
    getDIDevice()->EnumObjects(&EnumDeviceObjectsCallback, this, DIDFT_AXIS | DIDFT_BUTTON | DIDFT_POV);
    mEnumerated = true;
  }

  static BOOL CALLBACK EnumDeviceObjectsCallback(
    LPCDIDEVICEOBJECTINSTANCE lpddoi,
    LPVOID pvRef
  ) {
    static_cast<InputDevice*>(pvRef)->enumDeviceObjectsCallback(lpddoi);
    return DIENUM_CONTINUE;
  }

  void enumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE obj) {
    if (obj->guidType == GUID_POV) {
      mHats++;
      return;
    }
    if (obj->guidType == GUID_Button) {
      mButtons++;
      return;
    }

    if (obj->guidType == GUID_XAxis) {
      mAxes.push_back({AxisType::X, obj->tszName});
        return;
    }
    if (obj->guidType == GUID_YAxis) {
      mAxes.push_back({AxisType::Y, obj->tszName});
        return;
    }
    if (obj->guidType == GUID_ZAxis) {
      mAxes.push_back({AxisType::Z, obj->tszName});
      return;
    }

    if (obj->guidType == GUID_RxAxis) {
      mAxes.push_back({AxisType::RX, obj->tszName});
        return;
    }
    if (obj->guidType == GUID_RyAxis) {
      mAxes.push_back({AxisType::RY, obj->tszName});
      return;
    }
    if (obj->guidType == GUID_RzAxis) {
      mAxes.push_back({AxisType::RZ, obj->tszName});
      return;
    }

    if (obj->guidType == GUID_Slider) {
      mAxes.push_back({AxisType::SLIDER, obj->tszName});
      return;
    }
  }
};

void dump_state(InputDevice* device) {
  printf("State:\n");
  const auto buf = device->getState();
  const BYTE* data = buf.data();
  off_t offset = 0;
  for (int i = 1; i <= device->getAxisCount(); ++i) {
    printf("  Axis %d: %ld\n", i, *(long*)&buf[offset]);
    offset += sizeof(long);
  }
  for (int i = 1; i <= device->getHatCount(); ++i) {
    printf("  Hat %d: 0x%0.2x\n", i, *(BYTE*)&buf[offset]);
    offset += 1;
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

class DeviceManager {
 public:
  DeviceManager() : mDevices() {
    DirectInput8Create(
        GetModuleHandle(nullptr),
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (LPVOID*) &mDI,
        nullptr
    );
    mDI->EnumDevices(
        DI8DEVCLASS_GAMECTRL,
        &EnumDeviceCallback,
        this,
        DIEDFL_ATTACHEDONLY
    );
  }

  InputDevice* get(const VIDPID& vidpid) {
    for (auto device: mDevices) {
      auto dvp = device->getVIDPID();
      if (dvp && *dvp == vidpid) {
        return device;
      }
    }
    return nullptr;
  }
 private:
  IDirectInput8* mDI = nullptr;
  std::vector<InputDevice*> mDevices;

  static BOOL CALLBACK EnumDeviceCallback(
    LPCDIDEVICEINSTANCE device,
    LPVOID im
  ) {
    static_cast<DeviceManager*>(im)->enumDeviceCallback(device);
    return DIENUM_CONTINUE;
  }

  void enumDeviceCallback(
      LPCDIDEVICEINSTANCE didi
  ) {
    auto device = new InputDevice(mDI, didi);
    printf("Name: %s\n", device->getProductName().c_str());
    auto vidpid = device->getVIDPID();
    if (vidpid) {
      printf("VID: 0x%.4x\nPID: 0x%.4x\n", vidpid->vid, vidpid->pid);
    }
    printf("Axes: %lu\n", device->getAxisCount());
    for (const auto& axis: device->getAxesInformation()) {
      printf("  - %s\n", axis.name.c_str());
    }
    printf("Buttons: %lu\n", device->getButtonCount());
    printf("Hats: %lu\n", device->getHatCount());
    printf("---\n");
    mDevices.push_back(device);
  }
};

class EventSubscriber {
 public:
  virtual void onAxisEvent(InputDevice* device, int idx, long oldv, long newv) {}
  virtual void onHatEvent(InputDevice* device, int idx, BYTE oldv, BYTE newv) {}
  virtual void onButtonEvent(InputDevice* device, int idx, BYTE oldv, BYTE newv) {}
};

class EventDumper : public EventSubscriber {
 public:
  virtual void onAxisEvent(InputDevice* device, int idx, long oldv, long newv) {
    const auto max = std::numeric_limits<uint16_t>::max();
    // Don't print *all* the changes or it's super spammy
    if ((100 * oldv / max) == (100 * newv / max)) {
      return;
    }
    printf("Axis %d: %.3d%% -> %.3d%%\n", idx + 1, oldv * 100 / max, newv * 100 / max);
  }
  virtual void onHatEvent(InputDevice* device, int idx, BYTE oldv, BYTE newv) {
    printf("Hat %d: %x -> %x\n", idx + 1, oldv, newv);
  }
  virtual void onButtonEvent(InputDevice* device, int idx, BYTE oldv, BYTE newv) {
    printf("Button %d: %x -> %x\n", idx + 1, oldv, newv);
  }
};

class EventDispatcher {
 public:
  EventDispatcher(InputDevice* device): mDevice(device), mAxisCount(device->getAxisCount()), mHatCount(device->getHatCount()), mButtonCount(device->getButtonCount()), mState(device->getState()) {
 }
  void dispatch() {
    const auto new_state = mDevice->getState();
    const auto old_state = mState;
    assert(old_state.size() == new_state.size());
    mState = new_state;
    off_t offset = 0;
    for (int i = 0; i < mAxisCount; ++i) {
      const auto& old_value = *(long*)&old_state[offset];
      const auto& new_value = *(long*)&new_state[offset];
      if (old_value != new_value) {
        for (auto subscriber: mSubscribers) {
          subscriber->onAxisEvent(mDevice, i, old_value, new_value);
        }
      }
      offset += sizeof(long);
    }

    for (int i = 0; i < mHatCount; ++i) {
      const auto& old_value = *(BYTE*)&old_state[offset];
      const auto& new_value = *(BYTE*)&new_state[offset];
      if (old_value != new_value) {
        for (auto subscriber: mSubscribers) {
          subscriber->onHatEvent(mDevice, i, old_value, new_value);
        }
      }
      offset += 1;
    }

    for (int i = 0; i < mButtonCount; ++i) {
      const auto& old_value = *(BYTE*)&old_state[offset];
      const auto& new_value = *(BYTE*)&new_state[offset];
      if (old_value != new_value) {
        for (auto subscriber: mSubscribers) {
          subscriber->onButtonEvent(mDevice, i, old_value, new_value);
        }
      }
      offset += 1;
    }
  }
  void subscribe(EventSubscriber* subscriber) {
    mSubscribers.push_back(subscriber);
  }
 private:
  InputDevice* mDevice;
  size_t mAxisCount;
  size_t mHatCount;
  size_t mButtonCount;
  std::vector<BYTE> mState;
  std::vector<EventSubscriber*> mSubscribers;
};


int main() {
  DeviceManager devices;
  // auto device = devices.get({0x044f, 0x0404}); // throttle
  auto device = devices.get({0x3344, 0x40cc});
  if (!device) {
    printf("Failed to find device:'(\n");
    return 0;
  }
  printf("Found device: %s\n", device->getProductName().c_str());
  device->activate();
  auto event = device->getEvent();
  WaitForSingleObject(event, INFINITE);
  dump_state(device);
  EventDispatcher dispatcher(device);
  EventDumper dumper;
  dispatcher.subscribe(&dumper);
  while(true) {
    WaitForSingleObject(event, INFINITE);
    dispatcher.dispatch();
  }
  return 0;
}
