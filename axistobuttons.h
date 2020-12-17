#pragma once

#include "actionsapi.h"

#include <vector>
#include <cstdint>

namespace fredemmott::inputmapping {

class AxisToButtons : public AxisAction {
 public:
  struct Range {
    uint8_t minPercent;
    uint8_t maxPercent;
    ButtonTarget target;
  };

  AxisToButtons(std::initializer_list<Range> ranges);
  void map(long value);
  std::set<fredemmott::vjoypp::OutputDevice*> getAffectedDevices();
 private:
  struct RawRange {
    long min;
    long max;
    ButtonTarget target;
  };
  std::vector<RawRange> mRanges;
};

} // namespace fredemmott::inputmapping
