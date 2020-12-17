#include "mappabledevices.h"

#include "axistypes.h"
#include "inputdevice.h"
#include "vjoypp.h"

using fredemmott::gameinput::AxisType;
using fredemmott::gameinput::InputDevice;
using fredemmott::vjoypp::OutputDevice;

namespace fredemmott::inputmapping {

MappableOutputDevice::MappableOutputDevice(OutputDevice* dev):
  MappableDevice(dev),
#define A(x) x ## Axis { dev, #x, &OutputDevice::set ## x ## Axis }
  A(X), A(Y), A(Z),
  A(RX), A(RY), A(RZ),
#undef A
  Slider { dev, "Slider", &OutputDevice::setSlider},
  Dial { dev, "Dial", &OutputDevice::setDial}
{
}

namespace {
  AxisSource find_axis(InputDevice* device, AxisType t) {
    const auto info = device->getAxisInformation();
    for (uint8_t i = 0; i < info.size(); ++i) {
      if (info[i].type == t) {
        return {device, ++i };
      }
    }
    return {device, 0};
  }
} // namespace

MappableInputDevice::MappableInputDevice(InputDevice* dev):
  MappableDevice(dev),
#define A(x) x ## Axis(find_axis(dev, AxisType:: ## x ))
  A(X), A(Y), A(Z),
  A(RX), A(RY), A(RZ),
#undef A
  Slider(find_axis(dev, AxisType::SLIDER))
{
}

AxisSource MappableInputDevice::axis(uint8_t id) {
  return { device, id };
}
} // namespace fredemmott::inputmapping
