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

#include "UnsafeRef.h"

namespace fredemmott::inputmapping {

class Control {};

class Axis final : public Control {
 public:
  using Value = long;
  static const Value MAX = 0xffff;
  static const Value MIN = 0;
};
class Button final : public Control {
 public:
  using Value = bool;
};
class Hat final : public Control {
 public:
  using Value = uint16_t;
  static const Value MAX = 36000;
  static const Value MIN = 0;
};

class AnySink {};

template <typename TControl>
class Sink : public AnySink {
  static_assert(std::is_base_of_v<Control, TControl>);

 protected:
  Sink() = default;

 public:
  using InControl = TControl;
  using In = typename TControl::Value;
  virtual void map(In value) = 0;
};
using AxisSink = Sink<Axis>;
using ButtonSink = Sink<Button>;
using HatSink = Sink<Hat>;

template <typename TControl>
class FunctionSink : public Sink<TControl> {
 public:
  using Impl = std::function<void(typename TControl::Value)>;
  FunctionSink() = delete;
  FunctionSink(const Impl& impl) : impl(impl) {};
  virtual void map(typename TControl::Value value) override {
    impl(value);
  }

 private:
  Impl impl;
};

// Just so we can store a shared_ptr without worrying about the inner generics
class AnySource {
 protected:
  AnySource() = default;

 public:
  virtual ~AnySource();
};

template <typename TControl>
class Source : public AnySource {
  static_assert(std::is_base_of_v<Control, TControl>);

 public:
  using OutControl = TControl;
  using Out = typename TControl::Value;

  virtual void setNext(const UnsafeRef<Sink<TControl>>& next) {
    mNext = next;
  }

 protected:
  void emit(Out value) {
    if (!mNext) {
      return;
    }
    mNext->map(value);
  }

 private:
  UnsafeRef<Sink<TControl>> mNext;
};

using AxisSource = Source<Axis>;
using ButtonSource = Source<Button>;
using HatSource = Source<Hat>;

template <typename TIn, typename TOut>
class FunctionTransform final : public Sink<TIn>, public Source<TOut> {
  static_assert(std::is_base_of_v<Control, TIn>);
  static_assert(std::is_base_of_v<Control, TOut>);

 public:
  using Impl = std::function<typename TOut::Value(typename TIn::Value)>;

  FunctionTransform() = delete;
  FunctionTransform(const Impl& impl) : impl(impl) {};
  void map(typename TIn::Value value) final override {
    emit(impl(value));
  };

 private:
  Impl impl;
};

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
class SourceRef final : public Source<TControl> {
 public:
  using element_type = Source<TControl>;
  using Impl = UnsafeRef<element_type>;
  SourceRef(const Impl& impl) : p(impl) {};
  virtual void setNext(const UnsafeRef<Sink<TControl>>& next) override {
    p->setNext(next);
  }

  operator UnsafeRef<Source<TControl>> () const {
    return p;
  }

 private:
  Impl p;
};
using AxisSourceRef = SourceRef<Axis>;
using ButtonSourceRef = SourceRef<Button>;
using HatSourceRef = SourceRef<Hat>;

template <typename TControl>
class SinkRef final : public Sink<TControl> {
 public:
  using element_type = Sink<TControl>;
  using Impl = UnsafeRef<element_type>;
  SinkRef(const Impl& impl) : p(impl) {};

  virtual void map(typename TControl::Value value) override {
    p->map(value);
  }

  operator UnsafeRef<Sink<TControl>> () const {
    return p;
  }

 private:
  Impl p;
};
using AxisSinkRef = SinkRef<Axis>;
using ButtonSinkRef = SinkRef<Button>;
using HatSinkRef = SinkRef<Hat>;

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

// TODO: use C++20 concepts instead
template <typename T, typename = void>
struct is_transform : std::false_type {};

template <typename T>
struct is_transform<
  T,
  std::enable_if_t<
    std::is_base_of_v<AnySink, T> && std::is_base_of_v<AnySource, T>>>
  : std::true_type {};

template <typename T>
constexpr typename is_transform<T>::value_type is_transform_v
  = is_transform<T>::value;

template <typename T, typename = void>
struct is_sink : std::false_type {};
template <typename T>
struct is_sink<
  T,
  std::enable_if_t<
    std::is_base_of_v<AnySink, T> && !is_transform_v<T>>>
  : std::true_type {};

template <typename T>
constexpr typename is_sink<T>::value_type is_sink_v = is_sink<T>::value;

template <typename T, typename = void>
struct is_source: std::false_type {};
template <typename T>
struct is_source<
  T,
  std::enable_if_t<
    std::is_base_of_v<AnySource, T> && !is_transform_v<T>>>
  : std::true_type {};

template <typename T>
constexpr typename is_source<T>::value_type is_source_v = is_source<T>::value;

template<
	typename SourceRef,
	typename SinkRef,
	std::enable_if_t<is_source_v<typename SourceRef::element_type>, bool> = true,
	std::enable_if_t<is_sink_v<typename SinkRef::element_type>, bool> = true
>
Pipeline operator >> (
	SourceRef& left,
	const SinkRef& right 
) {
	left.setNext(right);
	return Pipeline(UnsafeRef<typename SourceRef::element_type>(left));
}

}// namespace fredemmott::inputmapping
