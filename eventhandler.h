#pragma once

#include "actionsapi.h"
#include "comboaction.h"
#include "passthroughs.h"

#include <memory>

namespace fredemmott::inputmapping {

/** A smart pointer to an Action, created from an Action or a Target.
 *
 * Targets are converted to Actions by creating a Passthrough.
 *
 * Targets and Actions are implicity convertable to EventHandlers.
 *
 * This is primarily to allow nice syntax when specifying
 * mappings.
 */
template<typename Target, typename Action, typename Passthrough>
class EventHandler {
public:
 EventHandler(const Target& target) {
   if (target.device) {
     mAction.reset(new Passthrough { target }) ;
   } else {
     mAction = nullptr;
    }
 }

 template<typename T>
 EventHandler(const T& action) {
   mAction = std::make_shared<T>(action);
 }

 typedef EventHandler<Target, Action, Passthrough> SelfType;
 EventHandler(const std::initializer_list<SelfType>& actions) {
   mAction = std::make_shared<ComboAction<SelfType, Action>>(actions);
 }

 operator bool() const {
   return (bool) mAction;
 }

 const std::shared_ptr<Action>& operator->() const {
   return mAction;
 }
private:
 std::shared_ptr<Action> mAction;
};

typedef EventHandler<AxisTarget, AxisAction, VJoyAxis>
  AxisEventHandler;
typedef EventHandler<ButtonTarget, ButtonAction, VJoyButton>
  ButtonEventHandler;
typedef EventHandler<HatTarget, HatAction, VJoyHat>
  HatEventHandler;

} // namespace fredemmott::inputmapping
