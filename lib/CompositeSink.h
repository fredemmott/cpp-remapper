/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "Sink.h"
#include "SinkRef.h"

namespace fredemmott::inputmapping::actions {

template<typename TControl>
class CompositeSink final : public Sink<TControl> {
 public:
  CompositeSink();
  void map(long value);

 private:
  std::vector<SinkRef<TControl>> mSinks;
};

}// namespace fredemmott::inputmapping::actions
