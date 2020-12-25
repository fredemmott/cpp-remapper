# What is this?

This project provides a concise API for remapping input devices (e.g.
joysticks) in C++.

It aims to:
- use modern standard C++ wherever possible
- have minimum dependencies

It uses vJoy (it is a vJoy 'feeder'), and supports HidGuardian without
replugging devices (if ran as administrator and supported by hardware).

# What does a profile look like?

``` C++
#include "easymode.h"

int main() {
  auto [p, throttle, stick, vj1, vj2] = create_profile(
    TM_WARTHOG_THROTTLE,
    VPC_RIGHT_WARBRD,
    VJOY_1,
    VJOY_2
  );

  // Copy all inputs to outputs
  p->passthrough(throttle, vj1);
  p->passthrough(stick, vj2);

  // Map a button
  p->map(throttle.Button1, vj1.Button1);

  // Let's do something a but more fun...

  // 1 physical button to 2 virtual buttons at the same time...
  p->map(throttle.Button1, {vj1.Button1, vj1.Button2} );

  // ... or one for short press, one for long press
  p->map(throttle.Button1, ShortPressLongPress { vj1.Button1, vj1.Button2 } );
  // ... with a custom duration for 'long press' (default is 300ms)
  p->map(
    throttle.Button1,
    ShortPressLongPress { vj1.Button1, vj1.Button2, std::chrono::seconds(1) }
  );

  // Convert an axis to buttons, e.g. turn a ministick
  // into 4-way buttons
  p->map(
    throttle.XAxis,
    AxisToButtons {
      { 0, 0, vj1.button(throttle.ButtonCount + 1) },
      { 100, 100, vj1.button(throttle.ButtonCount  + 2) }
    }
  );

  // Several at the same time.
  p->map({
    {
      throttle.XAxis,
      AxisToButtons {
        { 0, 0, vj1.button(throttle.ButtonCount + 1) },
        { 100, 100, vj1.button(throttle.ButtonCount  + 2) }
      },
    }, {
      throttle.YAxis,
      AxisToButtons {
        { 0, 0, vj1.button(throttle.ButtonCount + 3) },
        { 100, 100, vj1.button(throttle.ButtonCount + 4) }
      },
    }, {
      stick.RXAxis,
      AxisToButtons {
        { 0, 0, vj2.button(stick.ButtonCount + 1) },
        { 100, 100, vj2.button(stick.ButtonCount + 2) }
      },
    }, {
      stick.RYAxis,
      AxisToButtons {
        { 0, 0, vj2.button(stick.ButtonCount + 3) },
        { 100, 100, vj2.button(stick.ButtonCount + 4) }
      },
    },
  });

  p->run();
  return 0;
}
```

Later calls to `map()` replace any previous bindings for the specified source
button.

# How do I use this?

1. Download the vJoy SDK - the version must exactly match the version of vJoy
   you're using. As of 2020-12-18, the most recent version appears to be
   available from https://github.com/njz3/vJoy/
2. Extract it to the source directory so you have an `SDK/` subdirectory
3. Copy the `vJoyInterface.dll` from the SDK directory to the repo root directory
4. Install Visual Studio 2019 or newer
5. Open a terminal
6. `./build.ps1 example`
7. Run `example.exe`; run as administrator for HidGuardian support.
   - If you're in powershell: `Start-Process -Verb runas example.exe`

# How do I use this with another device?

Known devices are defined in [lib/devicedb.h](lib/devicedb.h). If your device
isn't listed there, you'll need it's ID; build `list-devices.exe` like the
example above:

```
PS > ./build.ps1 list-devices
PS > ./list-devices
"vJoy Device"
  Axes: 9
  Buttons: 64
  Hats: 1
  VID: 0x1234, PID: 0xBEA
  InstanceID: HID\HIDCLASS&Col01\1&4784345&2&0000
  HardwareID: HID\HIDCLASS&Col01
"T-Pendular-Rudder"
  Axes: 3
  Buttons: 0
  Hats: 0
  VID: 0x044F, PID: 0xB68
  InstanceID: HID\VID_044F&PID_B68F\7&63562dc&0&0000
  HardwareID: HID\VID_044F&PID_B68F
"RIGHT VPC Stick WarBRD"
  Axes: 6
  Buttons: 31
  Hats: 0
  VID: 0x3344, PID: 0x40C
  InstanceID: HID\VID_3344&PID_40CC&Col01\9&ccc01f0&1&0000
  HardwareID: HID\VID_3344&PID_40CC&Col01
"vJoy Device"
  Axes: 9
  Buttons: 64
  Hats: 1
  VID: 0x1234, PID: 0xBEA
  InstanceID: HID\HIDCLASS&Col02\1&4784345&2&0001
  HardwareID: HID\HIDCLASS&Col02
"Throttle - HOTAS Warthog"
  Axes: 5
  Buttons: 32
  Hats: 1
  VID: 0x044F, PID: 0x040
  InstanceID: HID\VID_044F&PID_0404\9&1e87ca54&1&0000
  HardwareID: HID\VID_044F&PID_0404
PS >
```

You can specify a device with:
- `VIDPID { 0x1234, 0x5678 }`: familiar, maybe not be supported by all HID
  devices, and may be ambiguous. Specifies the make/model of device.
- `HardwareID { "HID\\VID_1234&PID_5678" }` (using exact text from
  list-devices): supported by all devices. Usually specifies the make/model of
  the device, but may not.
- `InstanceID { "HID\\VID_1234&PID_678\somemorestuff" }`: specifies the specific
  instance of a device - usually not needed, but useful if you have multiple
  identical devices, e.g. HOSAS, button boxes, MFDs.

These can then be added to your code, as shown in `devicedb.h`.

# What actions are available?

- `AxisToButtons`
- `AxisToHat`
- `ShortPressLongPress`
- `SquareDeadzone`

There are two ways to do more: defining an action, or using
a lambda/function.

## AxisToHat

This class is different in that:
- it operates on two input axis
- it maintains state

It used like this:

```C++
AxisToHat myFirstHat(vjoy1.Hat1); // default deadzone
AxisToHat myHat(vjoy1.Hat1, 90); // custom deadzone, in percent
p->map(device.XAxis, myHat.XAxis);
p->map(device.YAxis, myHat.YAxis);
```

This creates a continuous (360-degree) hat out of the two axis.

The deadzone is calculated relative to a full deflection in a single axis, and
the deadzone distance-from-center is constant in all directions. This means
that:
- if it takes 1 inch of movement to pass the deadzone when moving directly
  right, it will take 1 inch of movement to pass the deadzone in any direction
- if the deadzone is 100%, it will require a full deflection for
  North, East, South, or West, but only a 71% deflection for NE, SE, SW, or SW,
  as (100%, 100%) is 141% the distance from center of (100%, 0%) or (0%, 100%).

## Using a lambda or function

If you wanted to use a lambda to invert button 1
(pressed -> unpressed, unpressed -> pressed):

```C++
p->map(real_device.Button1, [=](bool pressed) { vjoy.Button1.set(!pressed); });
```

Lambdas can be used in most places that accept a button; as an unrealistic example:

```
p->map(
  real_device.XAxis,
  AxisToButton { 0, 0, [=](bool pressed) { vjoy.Button1.set(!pressed); }
);
```

## Defining an action

An 'action' is an object that reacts to an input; you define an action by
creating a class that extends `fredemmott::inputmapping::AxisAction`,
`ButtonAction`, or `HatAction`, for example:

```C++
using namespace fredemmott::inputmapping;

class InvertButton final : public ButtonAction {
private:
  ButtonEventHandler mNext;
public:

  InvertButton(const ButtonEventHandler& next) : mNext(next) {}
  ~InvertButton() {}

  void map(bool value) {
    mNext->map(!value);
  }
};

int main() {
  // ...
  p.map(source.Button1, InvertButton { vjoy.Button1 } );
  // ...
}
```

`fredemmott::inputmapping::ButtonEventHandler` is an abstraction to allow
taking an action, lambda, or vJoy button identifier, for consistency
and to allow chaining.

If you implement additional actions, please consider contributing a pull request :)

## Pressing-and-releasing, batching, and time delays

All changes to the joystick actually happen at the same time, after all handlers
has been processed. This is good for maximizing performance and minimizing
latency, but means that if you change the same vjoy control twice, only the last
one happens.

For example:

```
vj1.Button1.set(true); // this has no effect...
vj1.Button1.set(false); // ... because this happened after
```

If you want to press-and-release a button, or do multiple actions over time, you
must inject them into the system:

```C++
#include "mapper"
#include <chrono>

void MyAction::map(bool value) {
  Mapper::inject(
    std::chrono::milliseconds(100), // how long to wait
    [=]() { do_something(value) };
  });
}
```

Do not use `sleep` or similar functions:
- they will block all mappings, including axis, until they are resolved
- the vjoy device will not be updated until your handler returns

## Continuous Hats

Continous hats are a `uint16_t`, with:
- centidegrees (i.e. 0 -> 35999) for actual values
- 0xffff for center

The Windows 'Game Controller' test page appears to treat them as signed
`int16_t`; this means that the Windows test page will not show continuous
hat values greater than 327.67 degrees. When testing your profile, use the
"Monitor vJoy" application.

# What support is available?

I make this for my own use, and I share this in the hope others find it useful;
I'm not able to commit to support, bug fixes, or feature development.

Please search for existing relevant discussions in the discussions tab on
GitHub, and create a new one if you don't find a relevant one.

I primarily work on this project to fit my needs, and am sharing in the hope
other's find it useful; I will not be able to spend large amounts of time
expanding it or in user support, but would greatly appreciate contributions :)

# LICENSE

The source code for this project is [MIT-licensed](LICENSE).

Because this is my personal repository, the license you receive to my code is
from me and not from my employer (Facebook).
