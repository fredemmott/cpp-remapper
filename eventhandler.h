#pragma once

#include "actionsapi.h"
#include "comboaction.h"
#include "passthroughs.h"

namespace fredemmott::inputmapping {

/** Given a Target or Action, get an Action.
 *
 * Targets are converted to Actions by creating a Passthrough.
 *
 * Targets and Actions are implicity convertable to OrTargets,
 * and OrTargets are implicitly convertable to Actions.
 *
 * This is primarily to allow nice syntax when specifying
 * mappings.
 */
template<typename Target, typename Action, typename Passthrough>
class EventHandler {
public:
 EventHandler(const Target& target) {
   if (target.device) {
     mAction = new Passthrough { target } ;
   } else {
     mAction = nullptr;
    }
 }
 template<typename T>
 EventHandler(const T& action) {
   mAction = new T(action);
 }

 typedef EventHandler<Target, Action, Passthrough> SelfType;
 EventHandler(const std::initializer_list<SelfType>& actions) {
   mAction = new ComboAction<SelfType, Action>(actions);
 }

 operator Action* () const {
   return mAction;
 }
 operator bool() const {
   return mAction;
 }
private:
 Action* mAction;
};

typedef EventHandler<AxisTarget, AxisAction, VJoyAxis>
  AxisEventHandler;
typedef EventHandler<ButtonTarget, ButtonAction, VJoyButton>
  ButtonEventHandler;
typedef EventHandler<HatTarget, HatAction, VJoyHat>
  HatEventHandler;

} // namespace fredemmott::inputmapping
