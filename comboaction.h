#pragma once

#include "actionsapi.h"
#include "actionortarget.h"

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

  virtual void map(typename Action::value_type value) {
    for (auto inner: mInners) {
      inner->map(value);
    }
  }

  std::set<vjoypp::OutputDevice*> getAffectedDevices() {
    std::set<vjoypp::OutputDevice*> out;
    for (const auto& inner: mInners) {
      out.merge(inner->getAffectedDevices());
    }
    return out;
  }

 private:
  std::vector<Action*> mInners;
};

} // namespace fredemmott::inputmapping::actions
