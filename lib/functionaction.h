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

#include <functional>

namespace fredemmott::inputmapping {

template<typename BaseAction>
class FunctionAction : public BaseAction {
public:
  typedef typename BaseAction::Value Value;
  typedef std::function<void(Value)> Callback;

  FunctionAction(Callback fun) : mFunction(fun) {}
  virtual ~FunctionAction() {}

  void map(Value value) {
    mFunction(value);
  }
private:
  Callback mFunction;
};

typedef FunctionAction<AxisAction> AxisFunctionAction;
typedef FunctionAction<ButtonAction> ButtonFunctionAction;
typedef FunctionAction<HatAction> HatFunctionAction;

} // namespace fredemmott::inputmapping
