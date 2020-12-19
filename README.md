# What is this?

This project provides a concise API for remapping input devices (e.g.
joysticks) in C++.

It aims to:
- use modern standard C++ wherever possible
- have minimum dependencies

It uses vJoy, and supports HidGuardian without replugging devices if ran
as administrator (tested with Thrustmaster and Virpil devices).

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
"T-Pendular-Rudder"
  VIDPID { 0x044f, 0xb68f }
  HID_ID { "HID\VID_044F&PID_B68F" }
  Axes: 3
  Buttons: 0
  Hats: 0
"Throttle - HOTAS Warthog"
  VIDPID { 0x044f, 0x0404 }
  HID_ID { "HID\VID_044F&PID_0404" }
  Axes: 5
  Buttons: 32
  Hats: 1
"RIGHT VPC Stick WarBRD"
  VIDPID { 0x3344, 0x40cc }
  HID_ID { "HID\VID_3344&PID_40CC&Col01" }
  Axes: 6
  Buttons: 31
  Hats: 0
"vJoy Device"
  VIDPID { 0x1234, 0xbead }
  HID_ID { "HID\HIDCLASS&Col01" }
  Axes: 9
  Buttons: 64
  Hats: 1
"vJoy Device"
  VIDPID { 0x1234, 0xbead }
  HID_ID { "HID\HIDCLASS&Col02" }
  Axes: 9
  Buttons: 64
  Hats: 1
PS >
```

The `HID_ID` struct definitions (actually Windows 'hardware IDs', in contrast
to 'device IDs' and 'instance IDs') are more specific, but most people are more
familiar with VID and PID.

The remapping will work with either, but the `HidGuardian` support may require
the `HID_ID` form if it contains additional data, like the HID collection
number in the VPC WarBRD's ID above.

These can then be added to your code, as shown in `devicedb.h`.

# What actions are available?

- `AxisToButton`
- `ShortPressLongPress`

There are two ways to do more: defining an action, or using
a lambda/function.

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
    [=]() { do_something(value);
  });
}
```

Do not use `sleep` or similar functions:
- they will block all mappings, including axis, until they are resolved
- the vjoy device will not be updated until your handler returns

# What support is available?

Please search for existing relevant discussions in the discussions tab on
GitHub, and create a new one if you don't find a relevant one.

I primarily work on this project to fit my needs, and am sharing in the hope
other's find it useful; I will not be able to spend large amounts of time
expanding it or in user support, but would greatly appreciate contributions :)

# LICENSE

The source code for this project is [MIT-licensed](LICENSE).

Because this is my personal repository, the license you receive to my code is
from me and not from my employer (Facebook).
