#pragma once

#include "actionsapi.h"
#include "eventhandler.h"

#include <vector>
#include <cstdint>

namespace fredemmott::inputmapping::actions {

class AxisToButtons : public AxisAction {
 public:
  struct Range {
    uint8_t minPercent;
    uint8_t maxPercent;
    ButtonEventHandler next;
  };

  AxisToButtons(std::initializer_list<Range> ranges);
  void map(long value);
 private:
  struct RawRange {
    long min;
    long max;
    ButtonEventHandler next;
  };
  std::vector<RawRange> mRanges;
};

} // namespace fredemmott::inputmapping
