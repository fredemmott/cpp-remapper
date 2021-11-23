/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>
#include <tuple>

namespace fredemmott::inputmapping::detail {

template<typename T>
struct function_traits : public function_traits<decltype(&T::operator())> {};

template<typename TClass, typename TRet, typename... TArgs>
struct function_traits<TRet(TClass::*)(TArgs...) const> :
	public function_traits<TRet(TArgs...)> {};

template<typename TRet, typename... TArgs>
struct function_traits<TRet(TArgs...)> {
  using ReturnType = TRet;
	using Signature = TRet(TArgs...);
  using ArgTypes = std::tuple<TArgs...>;
  template<std::size_t N>
  using ArgType = std::tuple_element_t<N, ArgTypes>;
  using FirstArg = ArgType<0>;
};

template<typename T, typename TSignature>
concept exact_signature
	= std::same_as<TSignature, typename function_traits<T>::Signature>;

template<typename T, typename... TArgs>
concept exact_arguments
	= std::same_as<std::tuple<TArgs...>, typename function_traits<T>::ArgTypes>;

template<typename T, typename TRet>
concept exact_return_type
	= std::same_as<TRet, typename function_traits<T>::ReturnType>;

}
