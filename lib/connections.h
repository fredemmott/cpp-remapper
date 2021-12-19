/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "ClosedPipeline.h"
#include "FunctionSink.h"
#include "FunctionTransform.h"
#include "SinkPipeline.h"
#include "SourcePipeline.h"
#include "TransformPipeline.h"
#include "TransformPtr.h"

namespace fredemmott::inputmapping {

class MappableInput;
class MappableVJoyOutput;

/// Convenience: copy an entire input device to vjoy
void operator>>(MappableInput&, const MappableVJoyOutput&);

/* Most of the 'how to tie stuff together' stuff works on '_ptr's, which is
 * essentially 'kinda looks like a `shared_ptr<>`.
 *
 * We work with `shared_ptr<>` where possible, but to support working with
 * lvalues, we also have `maybe_shared_ptr<T>`, which contains a raw pointer,
 * and an optional `shared_ptr<>` that must be equal to maintain the Ptrcount.'
 *
 * Let's start with the canonical Ptr-based operators:
 */

/// SourceOrTransformPtr >> SinkPtr
template <
  any_source_or_transform_ptr Left,
  sink_ptr<typename Left::element_type::OutControl> Right>
auto operator>>(Left left, const Right& right) {
  left->setNext(right);

  if constexpr (any_source_ptr<Left>) {
    return std::make_shared<ClosedPipeline>(left);
  }

  if constexpr (any_transform_ptr<Left>) {
    return std::make_shared<
      SinkPipeline<typename Left::element_type::InControl>>(left);
  }
}

/// SourceOrTransformPtr >> TransformPtr
template <
  any_source_or_transform_ptr Left,
  transform_from_ptr<typename Left::element_type::OutControl> Right>
auto operator>>(Left left, Right right) {
  left->setNext(right);

  if constexpr (any_source_ptr<Left>) {
    return std::make_shared<
      SourcePipeline<typename Right::element_type::OutControl>>(left, right);
  }

  if constexpr (any_transform_ptr<Left>) {
    return std::make_shared<TransformPipeline<
      typename Left::element_type::InControl,
      typename Right::element_type::OutControl>>(left, right);
  }
}

/* Okay, that's all combinations of Ptrs dealt with; now to wrap when
 * at least one side is not a Ptr */

/// SourceOrTransformPtr >> convertible_to_sink_ptr
template <
  any_source_or_transform_ptr Left,
  non_id_convertible_to_sink_ptr<typename Left::element_type::OutControl> Right>
auto operator>>(Left left, Right&& right) {
  return left >> convert_to_any_sink_ptr(std::forward<Right>(right));
}

/// SourceOrTransformPtr >> convertible_to_transform_ptr
template <
  any_source_or_transform_ptr Left,
  non_id_convertible_to_transform_from_ptr<
    typename Left::element_type::OutControl> Right>
auto operator>>(Left left, Right&& right) {
  return left >> convert_to_any_transform_ptr(std::forward<Right>(right));
}

/// convertible_to_source_ptr >> SinkOrTransformPtr
template <
  any_sink_or_transform_ptr Right,
  non_id_convertible_to_source_ptr<typename Right::element_type::InControl>
    Left>
auto operator>>(Left&& left, Right right) {
  return convert_to_any_source_ptr(std::forward<Left>(left)) >> right;
}

/// convertible_to_transform_ptr >> SinkOrTransformPtr
template <
  any_sink_or_transform_ptr Right,
  non_id_convertible_to_transform_to_ptr<
    typename Right::element_type::InControl> Left>
auto operator>>(Left&& left, Right right) {
  return convert_to_any_transform_ptr(std::forward<Left>(left)) >> right;
}

// clang-format 13 doesn't fully support C++20 concepts and requirements yet.
// clang-format off

/// convertible_to_source_ptr >> convertible_to_sink_or_transform_ptr
template <non_id_convertible_to_any_source_ptr Left, class Right>
requires
  (non_id_convertible_to_any_sink_ptr<Right> || non_id_convertible_to_any_transform_ptr<Right>)
  && requires(Left left, Right right) {
    convert_to_any_source_ptr(left) >> right;
  }
auto operator>>(Left&& left, Right&& right) {
  return convert_to_any_source_ptr(std::forward<Left>(left))
    >> std::forward<Right>(right);
}

/// convertible_to_transform_ptr >> convertible_to_sink_or_transform_ptr
template <non_id_convertible_to_any_transform_ptr Left, class Right>
requires
  (non_id_convertible_to_any_sink_ptr<Right> || non_id_convertible_to_any_transform_ptr<Right>)
  && requires(Left left, Right right) {
    convert_to_any_transform_ptr(left) >> right;
  }
auto operator>>(Left&& left, Right&& right) {
  return convert_to_any_transform_ptr(std::forward<Left>(left))
    >> std::forward<Right>(right);
}

// clang-format on

}// namespace fredemmott::inputmapping
