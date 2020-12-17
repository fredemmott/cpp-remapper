#pragma once

#include "actionsapi.h"

#include <cstdint>

namespace fredemmott::inputmapping {

template<typename TDevice>
struct MappableDevice {
 MappableDevice(TDevice* dev): device(dev) {}
 MappableButton<TDevice> button(uint8_t number) {
   return {device, number};
 }
 MappableHat<TDevice> hat(uint8_t number) {
   return {device, number};
 }

 TDevice* getDevice() const {
   return device;
 }
 protected:
  TDevice* device;
};

struct MappableOutputDevice : public MappableDevice<fredemmott::vjoypp::OutputDevice> {
  MappableOutputDevice(fredemmott::vjoypp::OutputDevice* dev);

  const AxisTarget XAxis;
  const AxisTarget YAxis;
  const AxisTarget ZAxis;
  const AxisTarget RXAxis;
  const AxisTarget RYAxis;
  const AxisTarget RZAxis;
  const AxisTarget Slider;
  const AxisTarget Dial;
};

struct MappableInputDevice : public MappableDevice<fredemmott::gameinput::InputDevice> {
  MappableInputDevice(fredemmott::gameinput::InputDevice* dev);

  const AxisSource XAxis;
  const AxisSource YAxis;
  const AxisSource ZAxis;
  const AxisSource RXAxis;
  const AxisSource RYAxis;
  const AxisSource RZAxis;
  const AxisSource Slider;

  AxisSource axis(uint8_t id);
};
} // namespace fredemmott::inputmapping
