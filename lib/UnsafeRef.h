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
  template<typename T>
  concept decay_equiv = std::same_as<T, std::decay_t<T>>;
}

template <detail::decay_equiv T>
class UnsafeRef {
  template<detail::decay_equiv U>
  friend class UnsafeRef;

  T* p {nullptr};
  std::shared_ptr<T> refcounted;

 public:
  using element_type = T;

  UnsafeRef() {
  }
  explicit UnsafeRef(T* impl) : p(impl) {
  }

  template <std::derived_from<T> TSubtype>
  UnsafeRef(const std::shared_ptr<TSubtype>& impl)
    : p(impl.get()), refcounted(impl) {
  }

  template <std::derived_from<T> TSubtype>
  UnsafeRef(const UnsafeRef<TSubtype>& impl)
    : p(impl.p), refcounted(impl.refcounted) {
  }

  template <std::derived_from<T> TConcrete>
  UnsafeRef(TConcrete&& temporary) {
    refcounted = std::make_shared<TConcrete>(std::move(temporary));
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
