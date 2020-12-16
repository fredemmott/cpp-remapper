#include "inputdevice.h"
#include "axistypes.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace fredemmott::gameinput {

bool VIDPID::operator==(const VIDPID& other) const {
  return vid == other.vid && pid == other.pid;
}

namespace {
  struct ControlInfo {
    uint32_t buttons = 0;
    uint32_t hats = 0;
    std::vector<AxisInformation> axes;
  };

  static BOOL CALLBACK enum_controls_callback(
    LPCDIDEVICEOBJECTINSTANCE obj,
    LPVOID pvRef
  ) {
    auto data = reinterpret_cast<ControlInfo*>(pvRef);
    if (obj->guidType == GUID_POV) {
      data->hats++;
      return DIENUM_CONTINUE;
    }
    if (obj->guidType == GUID_Button) {
      data->buttons++;
      return DIENUM_CONTINUE;
    }

    if (obj->guidType == GUID_XAxis) {
      data->axes.push_back({AxisType::X, obj->tszName});
        return DIENUM_CONTINUE;
    }
    if (obj->guidType == GUID_YAxis) {
      data->axes.push_back({AxisType::Y, obj->tszName});
        return DIENUM_CONTINUE;
    }
    if (obj->guidType == GUID_ZAxis) {
      data->axes.push_back({AxisType::Z, obj->tszName});
      return DIENUM_CONTINUE;
    }

    if (obj->guidType == GUID_RxAxis) {
      data->axes.push_back({AxisType::RX, obj->tszName});
        return DIENUM_CONTINUE;
    }
    if (obj->guidType == GUID_RyAxis) {
      data->axes.push_back({AxisType::RY, obj->tszName});
      return DIENUM_CONTINUE;
    }
    if (obj->guidType == GUID_RzAxis) {
      data->axes.push_back({AxisType::RZ, obj->tszName});
      return DIENUM_CONTINUE;
    }

    if (obj->guidType == GUID_Slider) {
      data->axes.push_back({AxisType::SLIDER, obj->tszName});
      return DIENUM_CONTINUE;
    }
    return DIENUM_CONTINUE;
  }
} // namespace

struct InputDevice::Impl {
  ControlInfo controlsData;
  IDirectInput8* di8;
  DIDEVICEINSTANCE device;
  LPDIRECTINPUTDEVICE8 diDevice;
  bool enumerated;

  LPDIRECTINPUTDEVICE8 getDIDevice() {
    if (diDevice) {
      return diDevice;
    }
    di8->CreateDevice(device.guidInstance, &diDevice, nullptr);
    return diDevice;
  }

  const ControlInfo& controls() {
    if (enumerated) {
      return controlsData;
    }
    getDIDevice()->EnumObjects(&enum_controls_callback, &controlsData, DIDFT_AXIS | DIDFT_BUTTON | DIDFT_POV);
    enumerated = true;
    return controlsData;
  }
};

InputDevice::InputDevice(IDirectInput8* di, LPCDIDEVICEINSTANCE device): p(new Impl { {}, di, *device, nullptr, false}) {
}

InputDevice::~InputDevice() {
}

std::string InputDevice::getInstanceName() const {
  return p->device.tszInstanceName;
}

std::string InputDevice::getProductName() const {
  return p->device.tszProductName;
}

std::optional<VIDPID> InputDevice::getVIDPID() const {
  const auto VID_PID_MAGIC = "\0\0PIDVID";
  const auto& guid = p->device.guidProduct;
  if (*(uint64_t*) guid.Data4 == *(uint64_t*)(VID_PID_MAGIC)) {
    const uint16_t vid = guid.Data1 & 0xffff;
    const uint16_t pid = guid.Data1 >> 16;
    return {{vid, pid}};
  }
  return {};
}

uint32_t InputDevice::getAxisCount() {
  return getAxisInformation().size();
}

std::vector<AxisInformation> InputDevice::getAxisInformation() {
  return p->controls().axes;
}

uint32_t InputDevice::getButtonCount() {
  return p->controls().buttons;
}

uint32_t InputDevice::getHatCount() {
  return p->controls().hats;
}

HANDLE InputDevice::getEvent() {
  return mEventHandle;
}

void InputDevice::activate() {
  if (mActivated) {
    return;
  }
  const DWORD count = getAxisCount() + getHatCount() + getButtonCount();
  LPDIOBJECTDATAFORMAT df = new DIOBJECTDATAFORMAT[count];
  off_t offset = 0;
  off_t i = 0;
  const auto& controls = p->controls();
  for (const auto& axis: controls.axes) {
    df[i++] = {
      NULL,
      (DWORD) offset,
      DIDFT_AXIS | DIDFT_ANYINSTANCE,
      NULL,
    };
    offset += sizeof(LONG);
  }
  for (size_t j = 0; j < controls.hats; j++) {
    df[i++] = {
      NULL,
      (DWORD) offset,
      DIDFT_POV | DIDFT_ANYINSTANCE,
      NULL,
    };
    offset += sizeof(int32_t);
  }
  for (size_t j = 0; j < controls.buttons; j++) {
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

  p->diDevice->SetCooperativeLevel(NULL, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
  p->diDevice->SetDataFormat(&data);
  delete[] df;

  mEventHandle = CreateEvent(nullptr, false, false, nullptr);
  p->diDevice->SetEventNotification(mEventHandle);
  p->diDevice->Acquire();
}

std::vector<uint8_t> InputDevice::getState() {
  p->diDevice->Poll();
  std::vector<uint8_t> buf(mDataSize);
  p->diDevice->GetDeviceState(mDataSize, buf.data());
  return buf;
}


} // namespace fredemmott::gameinput
