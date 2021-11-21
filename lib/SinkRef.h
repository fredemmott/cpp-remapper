/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "Source.h"
#include "UnsafeRef.h"

namespace fredemmott::inputmapping {

template <typename TControl>
class SinkRef final : public Sink<TControl> {
 public:
  using element_type = Sink<TControl>;
  using Impl = UnsafeRef<element_type>;
  SinkRef(const Impl& impl) : p(impl) {};

  virtual void map(typename TControl::Value value) override {
    p->map(value);
  }

  operator UnsafeRef<Sink<TControl>>() const {
    return p;
  }

 private:
  Impl p;
};
using AxisSinkRef = SinkRef<Axis>;
using ButtonSinkRef = SinkRef<Button>;
using HatSinkRef = SinkRef<Hat>;

}// namespace fredemmott::inputmapping
