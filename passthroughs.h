#pragma once

#include "actionsapi.h"

namespace fredemmott::inputmapping {

/* You're unlikely to want to create these in a profile; they're created
 * automatically by the Mapper when an (Axis|Button|Hat)Target is specified,
 * to convert them to an (Axis|Button|Hat)Action
 */

class AxisPassthrough : public AxisAction {
  public:
    AxisPassthrough(const AxisTarget& target);
    void map(long value);
    std::set<fredemmott::vjoypp::OutputDevice*> getAffectedDevices();
  private:
    AxisTarget mTarget;
};

class ButtonPassthrough : public ButtonAction {
  public:
    ButtonPassthrough(const ButtonTarget& target);
    void map(bool value);
    std::set<fredemmott::vjoypp::OutputDevice*> getAffectedDevices();
  private:
    ButtonTarget mTarget;
};

class HatPassthrough : public HatAction {
  public:
    HatPassthrough(const HatTarget& target);
    void map(int16_t value);
    std::set<fredemmott::vjoypp::OutputDevice*> getAffectedDevices();
  private:
    HatTarget mTarget;
};

} // namespace fredemmott::inputmapping
