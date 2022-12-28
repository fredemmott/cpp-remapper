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

namespace fredemmott::inputmapping {

namespace detail {
template <typename T>
concept decay_equiv = std::same_as<T, std::decay_t<T>>;

template <typename T>
concept container = requires(T) {
  typename T::element_type;
};

template <typename T>
concept maybe_shared_ptr_target
  = std::same_as<T, std::decay_t<T>> && !container<T>;
}// namespace detail

template <detail::maybe_shared_ptr_target T>
class maybe_shared_ptr {
  template <detail::maybe_shared_ptr_target U>
  friend class maybe_shared_ptr;

  T* p {nullptr};
  std::shared_ptr<T> refcounted;

 public:
  using element_type = T;

  maybe_shared_ptr() {
  }

  explicit maybe_shared_ptr(T* impl) : p(impl) {
  }

  template <std::derived_from<T> TSubtype>
  maybe_shared_ptr(const std::shared_ptr<TSubtype>& impl)
    : p(impl.get()), refcounted(impl) {
  }

  template <std::derived_from<T> TSubtype>
  maybe_shared_ptr(const maybe_shared_ptr<TSubtype>& impl)
    : p(impl.p), refcounted(impl.refcounted) {
  }

  template <std::derived_from<T> TSubtype>
  maybe_shared_ptr(TSubtype& local) : p(&local) {
  }

  template <std::derived_from<T> TSubtype>
  explicit maybe_shared_ptr(TSubtype&& temporary) {
    refcounted = std::make_shared<TSubtype>(std::move(temporary));
    p = refcounted.get();
  }

  bool isValid() const {
    return p;
  }
  T& operator*() const {
    return *p;
  }
  T* operator->() const {
    return p;
  }
};
}// namespace fredemmott::inputmapping
