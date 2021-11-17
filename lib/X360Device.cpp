/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "X360Device.h"

#include <iostream>

namespace fredemmott::inputmapping {

namespace {
  class ViGEmClient {
    public:
      ViGEmClient() {}
      ~ViGEmClient() {
        if (!p) {
          return;
        }
        std::cout << "Disconnecting from ViGEm bus..." << std::endl;
        vigem_disconnect(p);
        vigem_free(p);
        std::cout << "...disconnected." << std::endl;
      }

      operator PVIGEM_CLIENT () {
        if (p) {
          return p;
        }
        p = vigem_alloc();

				if (!p) {
					std::cerr << "Failed to allocate ViGEmClient" << std::endl;
					return nullptr;
				}

				auto ret = vigem_connect(p);
				if (!VIGEM_SUCCESS(ret)) {
					std::cerr << "ViGEm Bus connection failed with error code: 0x" << std::hex << ret << std::endl;
					vigem_free(p);
					p = nullptr;
					return nullptr;
				}

        return p;
      }

      operator bool () {
        return this->operator PVIGEM_CLIENT();
      }

      ViGEmClient(const ViGEmClient&) = delete;
      ViGEmClient& operator=(const ViGEmClient&) = delete;
    private:
      PVIGEM_CLIENT p = nullptr;
  };

  ViGEmClient g_client;
} // namespace

class X360Device::Impl {
  public:
    XUSB_REPORT state;
    PVIGEM_TARGET pad = nullptr;

    Impl() {
      XUSB_REPORT_INIT(&state);
      if (!g_client) {
        return;
      }
      pad = vigem_target_x360_alloc();
      auto ret = vigem_target_add(g_client, pad);
      if (!VIGEM_SUCCESS(ret)) {
        vigem_target_free(pad);
        pad = nullptr;
        std::cerr << "Failed to plug ViGEm pad: 0x" << std::hex << ret << std::endl;
      }
    }

    ~Impl() {
      if (!pad) {
        return;
      }
      std::cout << "Unplugging ViGEm X360 pad..." << std::endl;
      vigem_target_remove(g_client, pad);
      vigem_target_free(pad);
      std::cout << "...unplugged." << std::endl;
    }
};

X360Device::X360Device(): p(new Impl()) {
  if (p) {
    std::cout << "Attached ViGEm X360 pad." << std::endl;
    return;
  }

  std::cerr << "---\nERROR\n---\nFailed to initialize ViGEm X360 pad, output device will be missing.\n---" << std::endl;
}

X360Device::~X360Device() {}

void X360Device::flush() {
  if (!g_client) {
    return;
  }
  vigem_target_x360_update(g_client, p->pad, p->state);
}

X360Device* X360Device::setButton(Button button, bool value) {
  if (value) {
    p->state.wButtons |= (USHORT) button;
  } else {
    p->state.wButtons &= ~(USHORT) button;
  }
  return this;
}

X360Device* X360Device::setLXAxis(long value) {
  p->state.sThumbLX = value - 32768;
  return this;
}
X360Device* X360Device::setLYAxis(long value) {
  p->state.sThumbLY = 32767 - value;
  return this;
}
X360Device* X360Device::setRXAxis(long value) {
  p->state.sThumbRX = value - 32768;
  return this;
}
X360Device* X360Device::setRYAxis(long value) {
  p->state.sThumbRY = 32767 - value;
  return this;
}

X360Device* X360Device::setLTrigger(long value) {
  p->state.bLeftTrigger = (value * 255) / 65535;
  return this;
}
X360Device* X360Device::setRTrigger(long value) {
  p->state.bRightTrigger = (value * 255) / 65535;
  return this;
}

} // namespace fredemmott::inputmapping
