# What is this?

This project provides a concise API for remapping input devices (e.g.
joysticks) in C++.

It aims to:
- use modern standard C++ wherever possible; C++20 is required.
- have minimum dependencies

It integrates with:
- vJoy for virtual DirectInput controllers
- [ViGEmBus](https://github.com/ViGEm/ViGEmBus) for virtual XBox 360 and DualShock 4 controllers
- [HidHide](https://github.com/ViGEm/HidHide) for automatically hiding remapped controllers from games

# What does a profile look like?

``` C++
#include "easymode.h"

int main() {
  auto [p, throttle, stick, vj1, vj2, x360, ds4] = create_profile(
    TM_WARTHOG_THROTTLE,
    VPC_RIGHT_WARBRD,
    VJOY_1,
    VJOY_2,
    VIGEM_X360_PAD,
    VIGEM_DS4_PAD,
  );

  // Copy all inputs to outputs
  p->passthrough(throttle, vj1);
  p->passthrough(stick, vj2);

  // Map a button
  throttle.Button1 >> vj1.Button1;

  // Let's do something a bit more fun...

  // 1 physical button to 2 virtual buttons at the same time...
  throttle.Button1 >> all(vj1.Button1, vj1.Button2);

  // ... or one for short press, one for long press
  throttle.Button1 >> ShortPressLongPress { vj1.Button1, vj1.Button2 };
  // ... with a custom duration for 'long press' (default is 300ms)
  throttle.Button1 >> ShortPressLongPress
    throttle.Button1,
    ShortPressLongPress { vj1.Button1, vj1.Button2, std::chrono::seconds(1) }
  );

  // Convert an axis to buttons, e.g. turn a ministick
  // into 4-way buttons
  throttle.XAxis >>
    AxisToButtons {
      { 0, 0, vj1.button(throttle.getButtonCount() + 1) },
      { 100, 100, vj1.button(throttle.getButtonCount()  + 2) }
    }
  );

  // Let's invert an axis, then apply a deadzone and curve
  throttle.YAxis
    >> [](Axis::Value value) { return Axis::MAX - value; }
    >> AxisDeadzone(10)
    >> AxisCurve(0.5)
    >> vj1.YAxis;

  p->run();
  return 0;
}
```


# How do I use this?

1. Download the vJoy SDK - the version must exactly match the version of vJoy
   you're using. As of 2020-12-18, the most recent version appears to be
   available from https://github.com/njz3/vJoy/
2. Extract it to the source directory so you have an `SDK/` subdirectory
3. Copy the `vJoyInterface.dll` from the SDK directory to the repo root directory
4. Install Visual Studio 2019 or newer
5. Open a terminal
6. `./build.ps1 example.cpp`
7. Run `example.exe`; run as administrator for HidHide support.
   - If you're in powershell: `Start-Process -Verb runas example.exe`

# How do I use this with another device?

Known devices are defined in [lib/devicedb.h](lib/devicedb.h). If your device
isn't listed there, you'll need it's ID; build `list-devices.exe` like the
example above:

```
PS > ./build.ps1 list-devices.cpp
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

- `AxisCurve`
- `AxisToButtons`
- `AxisToHat`
- `ButtonToAxis`
- `ShortPressLongPress`
- `SquareDeadzone`

There are two ways to do more: defining an action, or using
a lambda/function.

## AxisCurve

This applies an S-shaped curve to an axis.

The 'curviness' is tunable (-1 < curviness < 1); values between 0 and 1 make it
less sensitive near the center (usually what is wanted), while values between
-1 and 0 make it more sensitive near the center.

A curviness of 0 produces a straight line; as the values approach 1 (or -1), the
curve approximates a rounded right angled corner. You probably want to start
around 0.5 and work from there.

It is used like this:

```C++
device.XAxis >> AxisCurve(0.5) >> vjoy1.XAxis;
device.YAxis >> AxisCurve(0.5) >> vjoy1.YAxis;
```

## AxisToHat

This class is different in that:
- it operates on two input axis
- it maintains state

It used like this:

```C++
AxisToHat myFirstHat(vjoy1.Hat1); // default deadzone
AxisToHat myHat(vjoy1.Hat1, 90); // custom deadzone, in percent
device.XAxis >> &myHat.XAxis;
device.YAxis >> &myHat.YAxis;
myHat >> vjoy1.Hat1;
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

## ButtonToAxis

This converts a button to an axis; this axis is always either 0 (if the button is not pressed), or at the maximum value (if the button is pressed). This can be useful for binding buttons to X360 controller triggers, which are axis. For example:

```C++
stick.Button1 >> ButtonToAxis() >> x360.RTrigger;
```

## Using a lambda or function

If you wanted to use a lambda to invert button 1
(pressed -> unpressed, unpressed -> pressed):

```C++
real_device.Button1 >> [](Button::Value pressed) { vjoy.Button1.set(!pressed); };
```

Lambdas can be used in most places that accept a button; as an unrealistic example:

```
real_device.XAxis >> AxisToButton({ 0, 0, [=](bool pressed) { vjoy.Button1.set(!pressed); });
```

## Defining an action

There are two basic kinds of admins
- a `Source<T>`: emits values; this is usually an axis/button/hat on a physical device
- a `Sink<T>`: receives values; this is usually an axis/button/hat on a vJoy or ViGEmBus device

`T` is `Axis`, `Button`, or `Hat`.

Most actions are both `Source`s and `Sink`s, and can be thought of as transformations.
Transformations can have the same input and output type, but this is not required. For example:

- `AxisCurve` is a `Sink<Axis>` and a `Source<Axis>`
- `ButtonToAxis` is a `Sink<Button>` and a `Source<Axis>`

```C++
using namespace fredemmott::inputmapping;

class InvertButton final : public Sink<Button>, public Source<Button> {
public:

  InvertButton() {}
  virtual ~InvertButton() {}

  void map(bool value) {
    emit(!value);
  }
};

int main() {
  // ...
  source.Button1 >> InvertButton >> vjoy.Button1;
  // ...
}
```

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

Continous hats are a `Hat::Type` (`uint16_t`), with:
- centidegrees (i.e. 0 -> 35999) for actual values
- `Hat::CENTER` (`0xffff`) for center

The Windows 'Game Controller' test page appears to treat them as signed
`int16_t`; this means that the Windows test page will not show continuous
hat values greater than 327.67 degrees. When testing your profile, use alternatives like:

- "Monitor vJoy"
- "VPC Joystick Tester"
- https://gamepad-tester.com for X360 outputs

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
