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
class SourceRef final : public Source<TControl> {
 public:
  using element_type = Source<TControl>;
  using Impl = UnsafeRef<element_type>;
  SourceRef(const Impl& impl) : p(impl) {};
  virtual void setNext(const UnsafeRef<Sink<TControl>>& next) override {
    p->setNext(next);
  }

  operator UnsafeRef<Source<TControl>>() const {
    return p;
  }

 private:
  Impl p;
};
using AxisSourceRef = SourceRef<Axis>;
using ButtonSourceRef = SourceRef<Button>;
using HatSourceRef = SourceRef<Hat>;

}// namespace fredemmott::inputmapping
