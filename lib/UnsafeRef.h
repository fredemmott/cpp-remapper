/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

namespace fredemmott::inputmapping {

template <typename T>
class UnsafeRef final {
  friend class UnsafeRef;
  T* p {nullptr};
  std::shared_ptr<T> refcounted;

 public:
  using element_type = T;

  UnsafeRef() {
  }
  explicit UnsafeRef(T* impl) : p(impl) {
  }
  UnsafeRef(const std::shared_ptr<T>& impl)
    : p(impl.get()), refcounted(impl) {
  }
  template <
    typename TConcrete,
    std::enable_if_t<!std::is_reference_v<TConcrete>, bool> = true,
    std::enable_if_t<std::is_base_of_v<T, TConcrete>, bool> = true>
  UnsafeRef(TConcrete&& temporary) {
    refcounted = std::make_shared<T>(std::move(temporary));
    p = refcounted.get();
  }

  operator bool() const {
    return p;
  }
  T& operator*() const {
    return *p;
  }
  T* operator->() const {
    return p;
  }

  template <
    typename Supertype,
    std::enable_if_t<std::is_base_of_v<Supertype, T>, bool> = true>
  operator UnsafeRef<Supertype> () const {
    UnsafeRef<Supertype> s;
    s.p = p;
    s.refcounted = refcounted;
    return s;
  }

  template <
    typename Subtype,
    std::enable_if_t<std::is_base_of_v<T, Subtype>, bool> = true>
  UnsafeRef<T>& operator=(const std::shared_ptr<Subtype>& other) {
    refcounted = other;
    p = refcounted.get();
    return *this;
  }
};

}// namespace fredemmott::inputmapping
