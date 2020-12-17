#pragma once

#include "actionsapi.h"
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
class ActionOrTarget {
public:
 ActionOrTarget(const Target& target) {
   mAction = new Passthrough { target } ;
 }
 template<typename T>
 ActionOrTarget(const T& action) {
   mAction = new T(action);
 }
 operator Action* () const {
   return mAction;
 }
private:
 Action* mAction;
};

class AxisActionOrTarget : public ActionOrTarget<AxisTarget, AxisAction, AxisPassthrough> {};
class ButtonActionOrTarget : public ActionOrTarget<ButtonTarget, ButtonAction, ButtonPassthrough> {};
class HatActionOrTarget : public ActionOrTarget<HatTarget, HatAction, HatPassthrough> {};

} // namespace fredemmott::inputmapping
