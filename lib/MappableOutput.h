/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <memory>

namespace fredemmott::inputmapping {

class OutputDevice;

class MappableOutput {
  protected:
    MappableOutput();
  public:
    virtual ~MappableOutput();
    virtual std::shared_ptr<OutputDevice> getDevice() const = 0;
};

} // namespace fredemmott::inputmapping
