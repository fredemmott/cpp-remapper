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
#include "actionsapi.h"

namespace fredemmott::inputmapping {

///// SourceRef >> SinkRef /////
template <is_source_ref Left, is_sink_ref Right>
Pipeline operator>>(Left left, Right right) requires std::same_as<
  typename Left::element_type::OutControl,
  typename Right::element_type::InControl> {
  left->setNext(right);
  return Pipeline(UnsafeRef<typename Left::element_type>(left));
}

///// SourceRef >> SinkFunc /////
template <
  is_source_ref Left,
  typename OutControl = typename Left::element_type::OutControl,
  sink_invocable<OutControl> Right>
Pipeline operator>>(Left left, Right right) {
  auto sink = std::make_shared<FunctionSink<OutControl>>(right);
  left->setNext(sink);
  return Pipeline(UnsafeRef<typename Left::element_type>(left));
}

///// SourceRef >> TransformRef /////
template <
  is_source_ref Left,
  typename OutControl = typename Left::element_type::OutControl,
  transform_from_ref<OutControl> Right>
SourcePipeline<OutControl> operator>>(
  Left left,
  Right right) {
  left->setNext(right);
  return SourcePipeline<OutControl>(left, right);
}

///// SourceRef >> TransformFunc /////
template <
  is_source_ref Left,
  typename Control = typename Left::element_type::OutControl,
  transform_invocable<Control, Control> Right>
SourcePipeline<Control> operator>>(Left left, Right right) {
  auto t = std::make_shared<FunctionTransform<Control, Control>>(right);
  left->setNext(t);
  return SourcePipeline<Control>(left, t);
}

///// SourceRef >> Value* (handy for testing) /////
template <
  is_source_ref Left,
  std::same_as<typename Left::element_type::Out> Right>
Pipeline operator>>(Left left, Right* right) {
  return left >> [right](Right value) { *right = value; };
}

template <typename Left, typename Right>
concept joinable = requires(Left left, Right right) {
  left >> right;
};

///// Make any non-temporary/moved source work with any supported right ////
template <source_or_transform Left, typename Right>
auto operator>>(Left& left, Right right) requires
  joinable<UnsafeRef<Source<typename Left::OutControl>>, Right> {
  auto ref = UnsafeRef(&left);
  return ref >> right;
}

///// Make any temporary/moved source work with any supported right ////
template <source_or_transform Left, typename Right>
auto operator>>(Left&& left, Right right) requires
  joinable<SourceRef<typename Left::OutControl>, Right> {
  auto owned = std::make_shared<Left>(std::move(left));
  return owned >> right;
}

///// Make any temporary/movable right work with any supported left /////
template <typename Left, sink_or_transform Right>
auto operator>>(Left left, Right&& right) requires
  joinable<Left, UnsafeRef<Right>> {
  UnsafeRef<Right> ref(std::move(right));
  return left >> ref;
}

///// Make any right raw ptr work with any supported left /////
template <typename Left, sink_or_transform Right>
auto operator>>(Left left, Right* right) requires
  joinable<Left, UnsafeRef<Right>> {
  UnsafeRef<Right> ref(right);
  return left >> ref;
}

}// namespace fredemmott::inputmapping
