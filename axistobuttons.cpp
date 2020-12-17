#define NOMINMAX

#include "axistobuttons.h"

#include "vjoypp.h"

#include <limits>

using fredemmott::vjoypp::OutputDevice;

namespace fredemmott::inputmapping {

AxisToButtons::AxisToButtons(std::initializer_list<Range> ranges) {
  const long max = std::numeric_limits<uint16_t>::max();
  for (const auto& range: ranges) {
    mRanges.push_back({(range.minPercent * max) / 100, (range.maxPercent * max) / 100, range.target});
  }
}

void AxisToButtons::map(long value) {
  for (const auto& range: mRanges) {
    bool active = range.min <= value && range.max >= value;
    range.target.device->setButton(range.target.button, active);
  }
}
std::set<OutputDevice*> AxisToButtons::getAffectedDevices() {
  std::set<OutputDevice*> out;
  for (const auto& range: mRanges) {
    out.emplace(range.target.device);
  }
  return out;
}

} // namespace fredemmott::inputmapping
