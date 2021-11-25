/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <concepts>
#include <iostream>

#include "DS4Device.h"
#include "ViGEmClient.h"

namespace fredemmott::inputmapping::detail {

template <typename TState, typename TDerived>
class ViGEmTarget {
 public:
  TState state;
  PVIGEM_TARGET pad = nullptr;

  ViGEmTarget() {
    TDerived::initState(&state);
    auto& client = ViGEmClient::get();
    if (!client) {
      return;
    }
    pad = TDerived::allocTarget();
    auto ret = vigem_target_add(client, pad);
    if (!VIGEM_SUCCESS(ret)) {
      vigem_target_free(pad);
      pad = nullptr;
      std::cerr << "Failed to plug ViGEm " << TDerived::productShortName()
                << ": 0x" << std::hex << ret << std::endl;
    }
  }

  ~ViGEmTarget() {
    if (!pad) {
      return;
    }
    std::cout << "Unplugging ViGEm " << TDerived::productShortName() << "..."
              << std::endl;
    vigem_target_remove(ViGEmClient::get(), pad);
    vigem_target_free(pad);
    std::cout << "...unplugged." << std::endl;
  }
};

}// namespace fredemmott::inputmapping::detail
