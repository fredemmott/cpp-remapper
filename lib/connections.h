/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>

#include "ClosedPipeline.h"
#include "FunctionSink.h"
#include "FunctionTransform.h"
#include "SinkPipeline.h"
#include "SourcePipeline.h"
#include "TransformPipeline.h"
#include "TransformRef.h"
#include "UnsafeRef.h"// TODO: split out decay_equiv

namespace fredemmott::inputmapping {

// clang-format doesn't properly support C++20 "Concepts" yet.

// clang-format off

/* Most of the 'how to tie stuff together' stuff works on '_ref's, which is
 * essentially 'kinda looks like a `shared_ptr<>`.
 *
 * We work with `shared_ptr<>` where possible, but to support working with
 * lvalues, we also have `UnsafeRef<T>`, which contains a raw pointer, and
 * an optional `shared_ptr<>` that must be equal to maintain the refcount.'
 *
 * Let's start with the canonical ref-based operators:
 */

///// SourceOrTransformRef >> SinkRef /////
template <
  any_source_or_transform_ref Left,
  sink_ref<typename Left::element_type::OutControl> Right>
auto operator>>(Left left, const Right& right) {
  left->setNext(right);

  if constexpr (any_source_ref<Left>) {
    return std::make_shared<ClosedPipeline>(UnsafeRef<typename Left::element_type>(left));
  }

  if constexpr (any_transform_ref<Left>) {
    return std::make_shared<TransformPipeline>(left, right);
  }
}

///// SourceOrTransformRef >> SinkFunc /////
template <
  any_source_or_transform_ref Left,
  typename OutControl = typename Left::element_type::OutControl,
  sink_invocable<OutControl> Right>
auto operator>>(Left left, Right right) {
  auto sink = std::make_shared<FunctionSink<OutControl>>(right);
  left->setNext(sink);
  if constexpr (any_source_ref<Left>) {
    return std::make_shared<ClosedPipeline>(UnsafeRef<typename Left::element_type>(left));
  }
  if constexpr(any_transform_ref<Left>) {
    return std::make_shared<SinkPipeline<typename Left::element_type::OutControl>>(UnsafeRef<typename Left::element_type>(left));
  }
}

///// SourceOrTransformRef >> TransformRef /////
template <
  any_source_or_transform_ref Left,
  transform_from_ref<typename Left::element_type::OutControl> Right>
auto operator>>(Left left, Right right) {
  left->setNext(right);

  if constexpr (any_source_ref<Left>) {
    return SourcePipeline<typename Right::element_type::OutControl>(
      left, right);
  }

  if constexpr (any_transform_ref<Left>) {
    return TransformPipeline<
      typename Left::element_type::InControl,
      typename Right::element_type::OutControl>(left, right);
  }
}

///// SourceRef >> TransformFunc /////
template <
  any_source_ref Left,
  typename Control = typename Left::element_type::OutControl,
  transform_invocable<Control, Control> Right>
SourcePipeline<Control> operator>>(Left left, Right right) {
  auto t = std::make_shared<FunctionTransform<Control, Control>>(right);
  left->setNext(t);
  return SourcePipeline<Control>(left, t);
}

///// SourceOrTransformRef >> Value* (handy for testing) /////
template <
  any_source_or_transform_ref Left,
  std::same_as<typename Left::element_type::Out> Right>
auto operator>>(Left left, Right* right) {
  return left >> [right](Right value) { *right = value; };
}

template <typename Left, typename Right>
concept pipeable = requires(Left left, Right right) {
  left >> right;
};

/* Now we're onto dealing with stuff that isn't a ref.
 *
 * `detail::decay_equiv` means that `std::decay<T>` is the same type as `T`; for example,
 * `detail::decay_equiv(T&)` is false, as `T&` != `T`.
 *
 * The idea is that:
 * - if it's potentially stored elsewhere (e.g. in a local), keep using the original value
 * - otherwise, consider it disposable and `std::move` it to somewhere we own.
 *
 * `Foo&&` is an r-value reference, a.k.a. moved value a.k.a temporary. This is supported
 * to allow temporary transformations, e.g.
 * `stick.XAxis >> AxisCurve(0.5) >> mysink` - in this case, the `AxisCurve` is an
 * rvalue-reference. These are converted to refs by calling `std::make_shared<T>`.
 *
 * We need to be extremely careful to avoid takinng a `const T&`, as rvalue-references are
 * convertible to const references, but not to mutable references.
 *
 * `Foo&` is an lvalue-reference, and needed to support `stick.XAxis` in the previous
 * example, or anything stored in a local, e.g.
 * `auto mytransform = [](Axis::Value value) { return Axis::MAX - value; };` - using
 * `mytransform` requires it to be taken as a mutable reference.
 */

///// Support a ref that's stored in a local, e.g. axis >> [](){} /////
template <any_source_or_transform Left, typename Right>
auto operator>>(Left& left, Right&& right) requires
  pipeable<UnsafeRef<Source<typename Left::OutControl>>, Right> {
  static_assert(pipeable<UnsafeRef<Left>, Right>);
  return UnsafeRef(&left) >> std::move(right);
}


///// Support temporary/moved RHS /////
template <any_source_or_transform_ref Left, detail::decay_equiv Right>
auto operator>>(Left left, Right&& right) requires
  pipeable<Left, UnsafeRef<Right>> {
  return left >> std::make_shared<Right>(std::move(right));
}

///// Support pointer RHS /////
template <any_source_or_transform_ref Left, detail::decay_equiv Right>
auto operator>>(Left left, Right* right) requires
  pipeable<Left, UnsafeRef<Right>> {
  return left >> UnsafeRef(right);
}

///// Local left, temporary right, e.g. `stick.Axis >> AxisCurve` /////
template <detail::decay_equiv Left, detail::decay_equiv Right>
auto operator>>(Left& left, Right&& right) requires
  pipeable<UnsafeRef<Left>, UnsafeRef<Right>> {
  return UnsafeRef(&left) >> std::make_shared<Right>(std::move(right));
}

////// Local left, pointer RHS /////
template <detail::decay_equiv Left, detail::decay_equiv Right>
auto operator>>(Left& left, Right* right) requires
  pipeable<UnsafeRef<Left>, Right*> {
  return UnsafeRef(&left) >> right;
}

///// Temporary left, temporary ref-able right /////
template <detail::decay_equiv Left, detail::decay_equiv Right>
auto operator>>(Left&& left, Right&& right) requires
  pipeable<UnsafeRef<Left>, UnsafeRef<Right>> {
  return std::make_shared<Left>(std::move(left))
    >> std::make_shared<Right>(std::move(right));
}

///// Temporary left, temporary non-ref-able-right (e.g. lambda) /////
template <detail::decay_equiv Left, detail::decay_equiv Right>
auto operator>>(Left&& left, Right&& right) requires
  pipeable<UnsafeRef<Left>, Right> {
  return std::make_shared<Left>(std::move(left))
    >> std::move(right);
}

///// Temporary left, const ref right, e.g. foo >> vj1.XAxis /////
template <detail::decay_equiv Left, detail::decay_equiv Right>
auto operator>>(Left&& left, const Right& right) requires
  any_sink_or_transform_ref<Right>
  && pipeable<UnsafeRef<Left>, const Right&> {
  return std::make_shared<Left>(std::move(left)) >> right;
}

///// Temporary left, raw pointer right /////
template <detail::decay_equiv Left, detail::decay_equiv Right>
auto operator>>(Left&& left, Right* right) requires
  pipeable<UnsafeRef<Left>, Right*> {
  return std::make_shared<Left>(std::move(left)) >> right;
}

}// namespace fredemmott::inputmapping
