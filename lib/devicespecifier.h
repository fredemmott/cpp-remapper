/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <variant>

namespace fredemmott::gameinput {

class InputDevice;

// Add any subclasses to DeviceSpecifier::Impl typedef too.
class DeviceSpecifierBase {
  public:
    virtual ~DeviceSpecifierBase();
    virtual bool matches(const InputDevice& device) const = 0;
    virtual std::string getHumanReadable() const = 0;
};

class VIDPID final : public DeviceSpecifierBase {
  public:
    VIDPID(uint16_t vid, uint16_t pid): mVID(vid), mPID(pid) {}

    virtual ~VIDPID();
    virtual bool matches(const InputDevice& device) const override;
    virtual std::string getHumanReadable() const override;

    // allow `auto [vid, pid] = my_vidpid;`
    operator std::tuple<uint16_t, uint16_t>() const {
      return std::make_tuple(mVID, mPID);
    }
  private:
    uint16_t mVID;
    uint16_t mPID;
};

class StringBasedID: public DeviceSpecifierBase {
  public:
    StringBasedID(const std::string& id);
    virtual ~StringBasedID();

    // This is primarily here for HidHide; these should be considered
    // opaque identifiers
    std::string toString() const;
    std::wstring toWString() const;
  protected:
    std::string mID;
};

class HardwareID final : public StringBasedID {
 public:
  using StringBasedID::StringBasedID;
  virtual bool matches(const InputDevice& device) const override;
  virtual std::string getHumanReadable() const override;
};

class InstanceID final : public StringBasedID {
 public:
  using StringBasedID::StringBasedID;
  virtual bool matches(const InputDevice& device) const override;
  virtual std::string getHumanReadable() const override;
};

// Copyable wrapper around a DeviceSpecifierBase
class DeviceSpecifier final: public DeviceSpecifierBase {
  public:
    typedef std::variant<VIDPID, HardwareID, InstanceID> Impl;

    template<typename T> DeviceSpecifier(const T& val): p(val) {}

    virtual ~DeviceSpecifier();
    virtual bool matches(const InputDevice& device) const override;
    virtual std::string getHumanReadable() const override;
  private:
    Impl p;
};

[[deprecated("Use `DeviceSpecifier` instead of `DeviceID`")]]
typedef DeviceSpecifier DeviceID;

[[deprecated("Use `HardwareID` instead of `HID_ID`")]]
typedef HardwareID HID_ID;

} // namespace fredemmott::gameinput
