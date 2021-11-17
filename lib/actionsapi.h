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

namespace fredemmott::inputmapping
{

	class Axis final {
		public:
			using Value = long;
			static const Value MAX = 0xffff;
			static const Value MIN = 0;
	};
	class Button final {
		public:
			using Value = bool;
	};
	class Hat final {
		public:
			using Value = uint16_t;
			static const Value MAX = 36000;
			static const Value MIN = 0;
	};

  class AxisInput;
  class ButtonInput;
  class HatInput;

  template <typename TValue>
  class Sink
  {
  protected:
    Sink() = default;
  public:
    using Value = TValue;
    virtual void map(TValue value) = 0;
  };
  class AxisSink : public Sink<Axis::Value>
  {
  };
  class ButtonSink : public Sink<Button::Value>
  {
  };
  class HatSink : public Sink<Hat::Value>
  {
  };

  template <typename TSink>
  class FunctionSink : public TSink
  {
  public:
    using Value = typename TSink::Value;
    using Impl = std::function<void(Value)>;
    FunctionSink() = delete;
    FunctionSink(const Impl &impl) : impl(impl){};
    virtual void map(Value value) override
    {
      impl(value);
    };

  private:
    Impl impl;
  };

  template <typename TSink, typename TSource>
  class FunctionTransform : public TSink, public TSource
  {
  public:
    using In = typename TSink::Value;
    using Out = typename TSource::Value;
    using Impl = std::function<Out(In)>;

    FunctionTransform() = delete;
    FunctionTransform(const Impl &impl) : impl(impl){};
    virtual void map(In value) override
    {
      emit(impl(value));
    };

  private:
    Impl impl;
  };

  template<typename T>
  class UnsafeRef final {
      friend class UnsafeRef;
      T* p { nullptr };
      std::shared_ptr<T> refcounted;
    public:
      UnsafeRef() {}
      UnsafeRef(const std::shared_ptr<T>& impl): p(impl.get()), refcounted(impl){}
      explicit UnsafeRef(T* impl): p(impl) {}

      operator bool () const { return p; }
      T& operator*() const { return *p; }
      T* operator->() const { return p; }

      template<
        typename Supertype,
        std::enable_if_t<std::is_base_of_v<Supertype, T>, bool> = true
      >
      operator UnsafeRef<Supertype> () {
        UnsafeRef<Supertype> s;
        s.p = p;
        s.refcounted = refcounted;
        return s;
      }

      template<
        typename Subtype,
        std::enable_if_t<std::is_base_of_v<T, Subtype>, bool> = true
      >
      UnsafeRef<T>& operator=(const std::shared_ptr<Subtype>& other) {
        refcounted = other;
        p = refcounted.get();
        return *this;
      }
  };

  // Just so we can store a shared_ptr without worrying about the inner generics
  class SourceBase
  {
  protected:
    SourceBase() = default;
  public:
    virtual ~SourceBase();
  };

  template <typename TSink>
  class Source : public SourceBase
  {
    static_assert(std::is_base_of_v<Sink<typename TSink::Value>, TSink>);

  public:
    using Sink = TSink;
    using Value = typename TSink::Value;

    void setNext(const UnsafeRef<TSink> &next)
    {
      mNext = next;
    }
  protected:
    void emit(Value value)
    {
      if (!mNext) {
        return;
      }
      mNext->map(value);
    }

  private:
    UnsafeRef<TSink> mNext;
  };

  class AxisSource : public Source<AxisSink>
  {
  };
  class ButtonSource : public Source<ButtonSink>
  {
  };
  class HatSource : public Source<HatSink>
  {
  };

  template <
      typename THead,
      typename TDerived>
  class PipelineTail
  {
    static_assert(std::is_base_of_v<Sink<typename THead::Value>, THead>);

  public:
    using Sink = THead;
    PipelineTail() = delete;

    template<
      typename TCallable,
      std::enable_if_t<
        std::is_convertible_v<
          TCallable,
          std::function<void(typename THead::Value)>
        >,
        bool
      > = true
    >
    PipelineTail(TCallable impl) : mHead(std::make_shared<FunctionSink<Sink>>(impl)) {}

    template<
      typename TRef,
      std::enable_if_t<
        std::is_convertible_v<TRef, UnsafeRef<THead>>,
        bool
      > = true
    >
    PipelineTail(TRef head) : mHead(head) {}
    PipelineTail(THead* ptr): mHead(ptr) {}

    template <typename... TRest>
    PipelineTail(TDerived first, TRest... rest)
    {
      std::vector<TDerived> inners{first, rest...};
      mHead = std::make_shared<FunctionSink<Sink>>(
          [inners](typename THead::Value value)
          {
            for (auto inner : inners)
            {
              inner.map(value);
            }
          });
    }

    void map(typename THead::Value value) const
    {
      mHead->map(value);
    };

    UnsafeRef<THead> getHead() const
    {
      return mHead;
    }

  private:
    UnsafeRef<THead> mHead;
  };
  class AxisOutput final : public PipelineTail<AxisSink, AxisOutput>
  {
  public:
    using PipelineTail::PipelineTail;
  };
  class ButtonOutput final : public PipelineTail<ButtonSink, ButtonOutput>
  {
  public:
    using PipelineTail::PipelineTail;
  };
  class HatOutput final : public PipelineTail<HatSink, HatOutput>
  {
  public:
    using PipelineTail::PipelineTail;
  };

  class Pipeline final
  {
  public:
    Pipeline() = delete;
    explicit Pipeline(const UnsafeRef<SourceBase> &source) : mSource(source) {}

    template <
        typename THead,
        typename TTail>
    Pipeline(
        THead head,
        TTail tail) : Pipeline(head.getHead())
    {
      head.bind(tail);
    }

  private:
    UnsafeRef<SourceBase> mSource;
  };

  template <typename TTail, typename TOutput>
  class PipelineHead
  {
    static_assert(std::is_base_of_v<Source<typename TTail::Sink>, TTail>);

  public:
    using HeadPtr = UnsafeRef<SourceBase>;
    using Tail = TTail;
    using TailPtr = UnsafeRef<TTail>;

    PipelineHead() = delete;
    explicit PipelineHead(const std::string &error) : mError(error){};
    explicit PipelineHead(const HeadPtr &source, const TailPtr &tail) : mHead(source), mTail(tail){};
    explicit PipelineHead(TTail* tail): mHead(tail), mTail(tail) {}

    template<
      typename Tailish,
      std::enable_if_t<std::is_convertible_v<Tailish, TailPtr>, bool> = true
    >
    PipelineHead(Tailish&& tail) {
      UnsafeRef<Tail> ref(tail);
      mTail = ref;
      mHead =ref;
    }

    template<
      typename Tailish,
      std::enable_if_t<std::is_base_of_v<Tail, Tailish>, bool> = true
    >
    PipelineHead(const std::shared_ptr<Tailish>& tail) : mHead(tail), mTail(tail) {}
    template<
      typename Output,
      std::enable_if_t<!std::is_reference_v<Output>, bool> = true,
      std::enable_if_t<!std::is_pointer_v<Output>, bool> = true,
      std::enable_if_t<std::is_convertible_v<Output, TOutput>, bool> = true,
      std::enable_if_t<!std::is_convertible_v<Output, SourceBase>, bool> = true,
      std::enable_if_t<!std::is_convertible_v<Output, std::function<std::any(typename TTail::Sink::Value)>>, bool> = true
    >
    Pipeline bind(const Output& next) {
      mTail->setNext(TOutput(next).getHead());
      return Pipeline(mHead);
    }

#define DEFINE_INVOCABLE_TRANSFORM_TO(Input, Source, ValueType) \
    template< \
      typename Transform, \
      std::enable_if_t<std::is_convertible_v<Transform, std::function<ValueType(typename TTail::Sink::Value)>>, bool> = true, \
      std::enable_if_t<std::is_same_v<ValueType, std::result_of_t<Transform(typename TTail::Sink::Value)>>, bool> = true \
    > \
    Input bind(Transform next) { \
      auto ref = UnsafeRef(std::make_shared<FunctionTransform<typename TTail::Sink, Source>>(next)); \
      mTail->setNext(ref); \
      return Input(mHead, ref); \
    }
    DEFINE_INVOCABLE_TRANSFORM_TO(AxisInput, AxisSource, Axis::Value);
    DEFINE_INVOCABLE_TRANSFORM_TO(ButtonInput, ButtonSource, Button::Value);
    DEFINE_INVOCABLE_TRANSFORM_TO(HatInput, HatSource, Hat::Value);
#undef DEFINE_INVOCABLE_TRANSFORM_TO

#define DEFINE_SOURCE_SINK_TRANSFORM_TO(Input, Source, ValueType) \
    template< \
      typename Transform, \
      std::enable_if_t<!std::is_reference_v<Transform>, bool> = true, \
      std::enable_if_t<std::is_base_of_v<typename TOutput::Sink, Transform>, bool> = true, \
      std::enable_if_t<std::is_base_of_v<Source, Transform>, bool> = true \
    > \
    Input bind(Transform* next) { \
      auto ref = UnsafeRef(next); \
      mTail->setNext(ref); \
      return Input(mHead, ref); \
    }
    DEFINE_SOURCE_SINK_TRANSFORM_TO(AxisInput, AxisSource, Axis::Value);
    DEFINE_SOURCE_SINK_TRANSFORM_TO(ButtonInput, ButtonSource, Button::Value);
    DEFINE_SOURCE_SINK_TRANSFORM_TO(HatInput, HatSource, Hat::Value);
#undef DEFINE_SOURCE_SINK_TRANSFORM_TO
#define DEFINE_SOURCE_SINK_TRANSFORM_TO(Input, Source, ValueType) \
    template< \
      typename Transform, \
      std::enable_if_t<!std::is_reference_v<Transform>, bool> = true, \
      std::enable_if_t<std::is_base_of_v<typename TOutput::Sink, Transform>, bool> = true, \
      std::enable_if_t<std::is_base_of_v<Source, Transform>, bool> = true \
    > \
    Input bind(Transform&& next) { \
      auto ref = UnsafeRef(std::make_shared<Transform>(std::move(next))); \
      mTail->setNext(ref); \
      return Input(mHead, ref); \
    }
    DEFINE_SOURCE_SINK_TRANSFORM_TO(AxisInput, AxisSource, Axis::Value);
    DEFINE_SOURCE_SINK_TRANSFORM_TO(ButtonInput, ButtonSource, Button::Value);
    DEFINE_SOURCE_SINK_TRANSFORM_TO(HatInput, HatSource, Hat::Value);
#undef DEFINE_SOURCE_SINK_TRANSFORM_TO

    // Handy for tests
    Pipeline bind(typename TTail::Source::Value* ptr) {
      return bind([ptr](typename TTail::Source::Value value) { *ptr = value; });
    }

    template<typename T>
    auto operator >> (T* value) {
      return this->bind(value);
    }

    template<typename T>
    auto operator >> (T&& value) {
      return this->bind(std::move(value));
    }

  private:
    std::string mError;
    HeadPtr mHead;
    TailPtr mTail;
  };

  class AxisInput : public PipelineHead<AxisSource, AxisOutput>
  {
  public:
    using PipelineHead::PipelineHead;
  };
  class ButtonInput : public PipelineHead<ButtonSource, ButtonOutput>
  {
  public:
    using PipelineHead::PipelineHead;
  };
  class HatInput : public PipelineHead<HatSource, HatOutput>
  {
  public:
    using PipelineHead::PipelineHead;
  };

} // namespace fredemmott::inputmapping
