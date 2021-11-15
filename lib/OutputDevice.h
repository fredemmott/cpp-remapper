/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

namespace fredemmott::inputmapping
{

  class OutputDevice
  {
  public:
    virtual ~OutputDevice();
    virtual void flush() = 0;
  };

} // namespace fredemmott::inputmapping
