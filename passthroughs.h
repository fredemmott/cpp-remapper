#pragma once

#include "actionsapi.h"

namespace fredemmott::inputmapping {

/* You're unlikely to want to create these in a profile; they're created
 * automatically by the Mapper when an (Axis|Button|Hat)Target is specified,
 * to convert them to an (Axis|Button|Hat)Action
 */

class VJoyAxis : public AxisAction {
  public:
    VJoyAxis(const AxisTarget& target);
    void map(long value);
    std::set<fredemmott::vjoypp::OutputDevice*> getAffectedDevices();
  private:
    AxisTarget mTarget;
};

class VJoyButton : public ButtonAction {
  public:
    VJoyButton(const ButtonTarget& target);
    void map(bool value);
    std::set<fredemmott::vjoypp::OutputDevice*> getAffectedDevices();
  private:
    ButtonTarget mTarget;
};

class VJoyHat : public HatAction {
  public:
    VJoyHat(const HatTarget& target);
    void map(int16_t value);
    std::set<fredemmott::vjoypp::OutputDevice*> getAffectedDevices();
  private:
    HatTarget mTarget;
};

} // namespace fredemmott::inputmapping
