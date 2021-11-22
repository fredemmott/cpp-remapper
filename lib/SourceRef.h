/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>
#include <memory>

#include "Source.h"
#include "UnsafeRef.h"

namespace fredemmott::inputmapping {

template<std::derived_from<Control> TControl>
using SourceRef = std::shared_ptr<Source<TControl>>;
using AxisSourceRef = SourceRef<Axis>;
using ButtonSourceRef = SourceRef<Button>;
using HatSourceRef = SourceRef<Hat>;

// clang-format off
template<typename T>
concept any_source_ref =
  any_source<typename T::element_type>
  && std::convertible_to<T, UnsafeRef<typename T::element_type>>;

template<typename T, typename TControl>
concept source_ref =
  any_source_ref<T>
  && std::convertible_to<typename T::element_type, TControl>;
// clang-format on


}// namespace fredemmott::inputmapping
