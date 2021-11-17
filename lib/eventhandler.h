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

#include <memory>

namespace fredemmott::inputmapping {

/** A smart pointer to an Action, created from various reasonable things.
 *
 * - Actions are copied.
 * - Callables (e.g. lambdas, function references) are converted to an
 *   std::function<void(Value)>, then to a FunctionAction<Action>.
 *
 * This is primarily to allow
 * `map(ButtonSource, ... any reasonable thing ...)` without explicit
 * conversions, while predominantly using values instead of pointers for the
 * user-facing API.
 */

template<typename TAction>
class EventHandler {
public:
 typedef EventHandler<TAction> SelfType;
 typedef FunctionAction<TAction> CallbackAction;
 typedef typename CallbackAction::Callback Callback;

 operator bool() const {
   return (bool) mAction;
 }

 const std::shared_ptr<TAction>& operator->() const {
   return mAction;
 }

 EventHandler(decltype(nullptr)) {}

 EventHandler(const std::initializer_list<SelfType>& actions) {
   mAction = std::make_shared<ComboAction<SelfType, TAction>>(actions);
 }

 template<
   typename T,
   typename std::enable_if<std::is_base_of<TAction, T>::value>::type* = nullptr
 >
 EventHandler(const T& action) {
   mAction = std::make_shared<T>(action);
 }

 template<
   typename T,
   typename std::enable_if<std::is_base_of<TAction, T>::value>::type* = nullptr
 >
 EventHandler(const std::shared_ptr<T>& action): mAction(action) {
 }

 template<
   typename T,
   typename std::enable_if<std::is_convertible<T, Callback>::value>::type* = nullptr
 >
 EventHandler(const T& cb) {
   mAction = std::make_shared<CallbackAction>(cb);
 }

private:
 std::shared_ptr<TAction> mAction;
};

typedef EventHandler<AxisAction>
  AxisEventHandler;
typedef EventHandler<ButtonAction>
  ButtonEventHandler;
typedef EventHandler<HatAction>
  HatEventHandler;

} // namespace fredemmott::inputmapping
