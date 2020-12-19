/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"
#include "eventhandler.h"

#include <vector>

namespace fredemmott::inputmapping::actions {

template<typename Wrapper, typename Action>
class ComboAction : public Action {
 public:
  ComboAction(const std::initializer_list<Wrapper>& inners) {
    // This is an implicit type conversion from Wrapper to Action
    for (const auto& inner: inners) {
      auto wrapper = inner;
      mInners.push_back(wrapper);
    }
  }

  virtual void map(typename Action::Value value) {
    for (auto inner: mInners) {
      inner->map(value);
    }
  }

 private:
  std::vector<Wrapper> mInners;
};

} // namespace fredemmott::inputmapping::actions
