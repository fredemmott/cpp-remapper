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
  std::vector<Action*> mInners;
};

} // namespace fredemmott::inputmapping::actions
