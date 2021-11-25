/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "ViGEmClient.h"

#include <iostream>

namespace fredemmott::inputmapping {

namespace {
ViGEmClient g_client;
}

ViGEmClient::ViGEmClient() {
}
ViGEmClient::~ViGEmClient() {
  if (!p) {
    return;
  }
  std::cout << "Disconnecting from ViGEm bus..." << std::endl;
  vigem_disconnect(p);
  vigem_free(p);
  std::cout << "...disconnected." << std::endl;
}

ViGEmClient::operator PVIGEM_CLIENT() {
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
    std::cerr << "ViGEm Bus connection failed with error code: 0x" << std::hex
              << ret << std::endl;
    vigem_free(p);
    p = nullptr;
    return nullptr;
  }

  return p;
}

ViGEmClient::operator bool() {
  return this->operator PVIGEM_CLIENT();
}

ViGEmClient& ViGEmClient::get() {
  return g_client;
};

}// namespace fredemmott::inputmapping