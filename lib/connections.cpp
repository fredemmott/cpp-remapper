/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <cpp-remapper/MappableFAVHIDOutput.h>
#include <cpp-remapper/MappableInput.h>
#include <cpp-remapper/MappableVJoyOutput.h>
#include <cpp-remapper/connections.h>

namespace fredemmott::inputmapping {

namespace detail {
template <class T>
void AssignVJoyLike(MappableInput& s, const T& t) {
  s.XAxis >> t.XAxis;
  s.YAxis >> t.YAxis;
  s.ZAxis >> t.ZAxis;
  s.RXAxis >> t.RXAxis;
  s.RYAxis >> t.RYAxis;
  s.RZAxis >> t.RZAxis;
  s.Slider >> t.Slider;
  s.Dial >> t.Dial;
  for (int i = 1; i <= s.getButtonCount(); ++i) {
    s.button(i) >> t.button(i);
  }
  for (int i = 1; i <= s.getHatCount(); ++i) {
    s.hat(i) >> t.hat(i);
  }
}
}// namespace detail

void operator>>(MappableInput& s, const MappableFAVHIDOutput& t) {
  detail::AssignVJoyLike(s, t);
}

void operator>>(MappableInput& s, const MappableVJoyOutput& t) {
  detail::AssignVJoyLike(s, t);
}

}// namespace fredemmott::inputmapping
