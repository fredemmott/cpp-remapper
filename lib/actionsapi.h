/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <type_traits>

#include "Controls.h"
#include "FunctionSink.h" // remove?
#include "FunctionTransform.h" // remove?
#include "Sink.h"
#include "Source.h"
#include "SourceRef.h"
#include "SinkRef.h"
#include "UnsafeRef.h"

namespace fredemmott::inputmapping {

template <typename TControl>
class PipelineTail : public Sink<TControl> {
  static_assert(std::is_base_of_v<Control, TControl>);

 public:
  PipelineTail() = delete;

  // TODO: move to FunctionSink?
  template <
    typename TCallable,
    std::enable_if_t<
      std::is_convertible_v<
        TCallable,
        std::function<void(typename TControl::Value)>>,
      bool> = true>
  PipelineTail(TCallable impl)
    : mHead(std::make_shared<FunctionSink<TControl>>(impl)) {
  }

  PipelineTail(const UnsafeRef<Sink<TControl>>& head) : mHead(head) {
  }
  explicit PipelineTail(Sink<TControl>* ptr) : mHead(ptr) {
  }

  // TODO: move to ComboAction?
  template <typename TFirst, typename... TRest>
  PipelineTail(TFirst&& first, TRest&&... rest) {
    // FIXME
    /*
    std::vector<Sink<TControl>> inners {first, rest...};
    mHead = std::make_shared<FunctionSink<TControl>>(
      [inners](typename TControl::Value value) {
        for (auto& inner: inners) {
          inner.map(value);
        }
      });
      */
  }

  void map(typename TControl::Value value) override {
    mHead->map(value);
  };

  UnsafeRef<Sink<TControl>> getHead() const {
    return mHead;
  }

 private:
  UnsafeRef<Sink<TControl>> mHead;
};

using AxisOutput = PipelineTail<Axis>;
using ButtonOutput = PipelineTail<Button>;
using HatOutput = PipelineTail<Hat>;

class Pipeline final {
 public:
  Pipeline() = delete;
  template<typename T, std::enable_if_t<std::is_base_of_v<AnySource, T>, bool> = true>
  Pipeline(const UnsafeRef<T>& source) : mSource(source) {
  }

 private:
  UnsafeRef<AnySource> mSource;
};

template <typename TControl>
class PipelineHead final : public Source<TControl> {
 public:
  PipelineHead() = delete;
  PipelineHead(const UnsafeRef<Source<TControl>>& tail)
    : mHead(tail), mTail(tail) {
  }
  explicit PipelineHead(Source<TControl>* tail) : mHead(tail), mTail(tail) {
  }

  PipelineHead(
    const UnsafeRef<AnySource>& source,
    const UnsafeRef<Source<TControl>>& tail)
    : mHead(source), mTail(tail) {};

  template <
    typename Next,
    std::enable_if_t<!std::is_reference_v<Next>, bool> = true,
    std::enable_if_t<std::is_base_of_v<Sink<TControl>, Next>, bool> = true,
    std::enable_if_t<!std::is_base_of_v<AnySource, Next>, bool> = true>
  Pipeline bind(Next&& next) {
    auto ref = UnsafeRef(std::make_shared<Next>(std::move(next)));
    mTail->setNext(ref);
    return Pipeline(mHead);
  }

  template <
    typename Transform,
    std::enable_if_t<std::is_base_of_v<Sink<TControl>, Transform>, bool> = true,
    std::enable_if_t<std::is_base_of_v<AnySource, Transform>, bool> = true>
  PipelineHead<typename Transform::OutControl> bind(Transform* next) {
    auto ref = UnsafeRef(next);
    mTail->setNext(ref);
    return PipelineHead(mHead, ref);
  }

  template <
    typename Transform,
    std::enable_if_t<!std::is_reference_v<Transform>, bool> = true,
    std::enable_if_t<std::is_base_of_v<Sink<TControl>, Transform>, bool> = true,
    std::enable_if_t<std::is_base_of_v<AnySource, Transform>, bool> = true>
  PipelineHead<typename Transform::OutControl> bind(Transform&& next) {
    auto ref = UnsafeRef(std::make_shared<Transform>(std::move(next)));
    mTail->setNext(ref);
    return Input(mHead, ref);
  }

  // Handy for tests
  Pipeline bind(typename TControl::Value* ptr) {
    return bind([ptr](typename TControl::Value value) { *ptr = value; });
  }

  template <typename T>
  auto operator>>(T* value) {
    return this->bind(value);
  }

  template <typename T>
  auto operator>>(T&& value) {
    return this->bind(std::move(value));
  }

 private:
  UnsafeRef<AnySource> mHead;
  UnsafeRef<Source<TControl>> mTail;
};

using AxisInput = PipelineHead<Axis>;
using ButtonInput = PipelineHead<Button>;
using HatInput = PipelineHead<Hat>;

}// namespace fredemmott::inputmapping
