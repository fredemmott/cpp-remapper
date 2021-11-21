/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>

#include "Source.h"
#include "UnsafeRef.h"

namespace fredemmott::inputmapping {

template <typename TControl>
class SourceRef final : public Source<TControl> {
 public:
  using element_type = Source<TControl>;
  using Impl = UnsafeRef<element_type>;

  template<typename T>
  SourceRef(T impl) requires std::convertible_to<T, Impl> && (!std::convertible_to<typename T::element_type, AnySink>): p(impl) {};

  explicit SourceRef(Source<TControl>* impl) : p(impl) {}

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

// clang-format off
template<typename T>
// TODO: rename to source_or_ref
concept is_source_ref =
  is_source<typename T::element_type>
  && std::convertible_to<T, UnsafeRef<typename T::element_type>>;

template<typename T>
concept is_nonref_source =
  is_source<T>
  && !is_source_ref<T>;
// clang-format on


}// namespace fredemmott::inputmapping
