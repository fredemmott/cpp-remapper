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
    virtual bool matches(InputDevice* device) const = 0;
    virtual std::string getHumanReadable() const = 0;
};

class VIDPID final : public DeviceSpecifierBase {
  public:
    VIDPID(uint16_t vid, uint16_t pid): mVID(vid), mPID(pid) {}

    virtual ~VIDPID();
    virtual bool matches(InputDevice* device) const;
    virtual std::string getHumanReadable() const;

    // allow `auto [vid, pid] = my_vidpid;`
    operator std::tuple<uint16_t, uint16_t>() const {
      return std::make_tuple(mVID, mPID);
    }
  private:
    uint16_t mVID;
    uint16_t mPID;
};

class HID_ID final: public DeviceSpecifierBase {
  public:
    HID_ID(const std::string& id): mID(id) {}
    virtual ~HID_ID();
    virtual bool matches(InputDevice* device) const;
    virtual std::string getHumanReadable() const;

    operator std::string() const {
      return mID;
    }

    bool operator== (const HID_ID& other) const {
      return mID == other.mID;
    }

  private:
    std::string mID;
};

// Copyable wrapper around a DeviceSpecifierBase
class DeviceSpecifier final: public DeviceSpecifierBase {
  public:
    typedef std::variant<VIDPID, HID_ID> Impl;

    template<typename T> DeviceSpecifier(const T& val): p(val) {}

    virtual ~DeviceSpecifier();
    virtual bool matches(InputDevice* device) const;
    virtual std::string getHumanReadable() const;
  private:
    Impl p;
};

typedef DeviceSpecifier DeviceID;

} // namespace fredemmott::gameinput
