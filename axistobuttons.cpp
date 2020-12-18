#define NOMINMAX

#include "axistobuttons.h"

#include <limits>

namespace fredemmott::inputmapping::actions {

AxisToButtons::AxisToButtons(std::initializer_list<Range> ranges) {
  const long max = std::numeric_limits<uint16_t>::max();
  for (const auto& range: ranges) {
    mRanges.push_back({(range.minPercent * max) / 100, (range.maxPercent * max) / 100, range.next});
  }
}

void AxisToButtons::map(long value) {
  for (const auto& range: mRanges) {
    bool active = range.min <= value && range.max >= value;
    range.next->map(active);
  }
}
std::set<vjoypp::OutputDevice*> AxisToButtons::getAffectedDevices() {
  std::set<vjoypp::OutputDevice*> out;
  for (const auto& range: mRanges) {
    out.merge(range.next->getAffectedDevices());
  }
  return out;
}

} // namespace fredemmott::inputmapping
