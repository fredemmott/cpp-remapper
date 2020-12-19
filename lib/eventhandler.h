/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"
#include "comboaction.h"
#include "functionaction.h"
#include "passthroughs.h"

#include <memory>

namespace fredemmott::inputmapping {

/** A smart pointer to an Action, created from various reasonable things.
 *
 * - Actions are copied.
 * - Targets are converted to Actions by creating a Passthrough.
 * - Callables (e.g. lambdas, function references) are converted to an
 *   std::function<void(Value)>, then to a FunctionAction<Action>.
 *
 * This is primarily to allow
 * `map(ButtonSource, ... any reasonable thing ...)` without explicit
 * conversions, while predominantly using values instead of pointers for the
 * user-facing API.
 */
template<typename Target, typename Action, typename Passthrough>
class EventHandler {
public:
 typedef EventHandler<Target, Action, Passthrough> SelfType;
 typedef FunctionAction<Action> CallbackAction;
 typedef typename CallbackAction::Callback Callback;

 operator bool() const {
   return (bool) mAction;
 }

 const std::shared_ptr<Action>& operator->() const {
   return mAction;
 }

 EventHandler(const Target& target) {
   if (target.device) {
     mAction.reset(new Passthrough { target }) ;
   }
 }

 EventHandler(const std::initializer_list<SelfType>& actions) {
   mAction = std::make_shared<ComboAction<SelfType, Action>>(actions);
 }

 template<
   typename T,
   typename std::enable_if<std::is_base_of<Action, T>::value>::type* = nullptr
 >
 EventHandler(const T& action) {
   mAction = std::make_shared<T>(action);
 }

 template<
   typename T,
   typename std::enable_if<std::is_convertible<T, Callback>::value>::type* = nullptr
 >
 EventHandler(const T& cb) {
   mAction = std::make_shared<CallbackAction>(cb);
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
