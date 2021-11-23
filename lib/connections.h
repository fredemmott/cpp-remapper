/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>

#include "FunctionSink.h"
#include "FunctionTransform.h"
#include "TransformRef.h"
#include "UnsafeRef.h" // TODO: split out decay_equiv
#include "actionsapi.h"

namespace fredemmott::inputmapping {

///// SourceOrTransformRef >> SinkRef /////
template <
  any_source_or_transform_ref Left,
  typename OutControl = typename Left::element_type::OutControl,
  sink_ref<OutControl> Right>
auto operator>>(Left left, Right right) {
  left->setNext(right);

  if constexpr (any_source_ref<Left>) {
    return Pipeline(UnsafeRef<typename Left::element_type>(left));
  }

  if constexpr (any_transform_ref<Left>) {
    return TransformPipeline(left, right);
  }
}

///// SourceRef >> SinkFunc /////
template <
  any_source_ref Left,
  typename OutControl = typename Left::element_type::OutControl,
  sink_invocable<OutControl> Right>
Pipeline operator>>(Left left, Right right) {
  auto sink = std::make_shared<FunctionSink<OutControl>>(right);
  left->setNext(sink);
  return Pipeline(UnsafeRef<typename Left::element_type>(left));
}

///// SourceOrTransformRef >> TransformRef /////
template <
  any_source_or_transform_ref Left,
  typename OutControl = typename Left::element_type::OutControl,
  transform_from_ref<OutControl> Right>
auto operator>>(Left left, Right right) {
  left->setNext(right);

  if constexpr (any_source_ref<Left>) {
    return SourcePipeline<OutControl>(left, right);
  }

  if constexpr (any_transform_ref<Left>) {
    return TransformPipeline<
      OutControl,
      typename Right::element_type::InControl>(left, right);
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

///// SourceRef >> Value* (handy for testing) /////
template <
  any_source_ref Left,
  std::same_as<typename Left::element_type::Out> Right>
Pipeline operator>>(Left left, Right* right) {
  return left >> [right](Right value) { *right = value; };
}

template <typename Left, typename Right>
concept joinable = requires(Left left, Right right) {
  left >> right;
};

///// SourceRef >> Value* (handy for testing) /////
template <any_source_or_transform Left, typename Right>
auto operator>>(Left& left, Right&& right) requires
  joinable<UnsafeRef<Source<typename Left::OutControl>>, Right> {
  static_assert(joinable<UnsafeRef<Left>, Right>);
  auto ref = UnsafeRef(&left);
  return ref >> std::move(right);
}

///// Make any temporary/moved source work with any supported right ////
template <any_source_or_transform Left, typename Right>
auto operator>>(Left&& left, Right&& right) requires
  joinable<SourceRef<typename Left::OutControl>, Right> {
  auto owned = std::make_shared<Left>(std::move(left));
  return owned >> std::move(right);
}

///// Make any temporary/movable right work with any supported left /////
template <typename Left, any_sink_or_transform Right>
auto operator>>(Left left, Right&& right) requires
  joinable<Left, UnsafeRef<Right>> {
  UnsafeRef<Right> ref(std::move(right));
  return std::move(left) >> ref;
}

template <typename Left, any_sink_or_transform Right>
auto operator>>(Left left, Right* right) requires
  joinable<Left, UnsafeRef<Right>> {
  UnsafeRef<Right> ref(right);
  return std::move(left) >> ref;
}

}// namespace fredemmott::inputmapping
