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

  // Copy all inputs to outputs (only supported for VJoy outputs)
  throttle >> vj1;
  stick >> vj2;

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
      { 0_percent, 0_percent, vj1.button(throttle.getButtonCount() + 1) },
      { 100_percent, 100_percent, vj1.button(throttle.getButtonCount()  + 2) }
    }
  );

  // Let's invert an axis, then apply a deadzone and curve
  throttle.YAxis
    >> [](Axis::Value value) { return Axis::MAX - value; }
    >> AxisDeadzone(10)
    >> AxisCurve(0.5)
    >> vj1.YAxis;

  p.run();
  return 0;
}
```


# How do I use this?

1. Download the vJoy SDK - the version must exactly match the version of vJoy
   you're using. As of 2020-12-18, the most recent version appears to be
   available from https://github.com/njz3/vJoy/
2. Extract it to the `third-party/vJoy` directory so you have an `third-party/vJoy/SDK/` subdirectory
3. Copy the `vJoyInterface.dll` from the SDK directory to the `bin/` subdirectory
4. Install Visual Studio 2019 and `vswhere`
5. Open a terminal
6. `./build.ps1 example.cpp`
7. Run `bin/example.exe`

The ViGEM client build currently requires Visual Studio 2019; `cpp-remapper` itself can be built with VS2019, clang, or VS2022.

# HidHide

By default, all input devices that are used will be hidden from other programs. You can override this by wrapping the device specifier in `UnhiddenDevice`:

```c++
auto [p, throttle, vj1] = create_profile(
  UnhiddenDevice(TM_WARTHOG_THROTTLE),
  VJOY_1
);
```

This can be useful if you don't actually want to modify a device, but instead want to use it as a modifier for another device.

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
- `InstanceID { "HID\\VID_1234&PID_678\\somemorestuff" }`: specifies the specific
  instance of a device - usually not needed, but useful if you have multiple
  identical devices, e.g. HOSAS, button boxes, MFDs.

These can then be added to your code, as shown in `devicedb.h`.

# What actions are available?

- `AxisCurve`
- `AxisToButtons`
- `AxisToHat`
- `AxisTrimmer`
- `ButtonToAxis`
- `HatToButtons`
- `LatchedToMomentaryButton`
- `MomentaryToLatchedButton`
- `Shift`
- `ShortPressLongPress`
- `SquareDeadzone`

There are two ways to do more: defining an action, or using
a lambda/function.

Additionally, there are two helpers for combining/splitting controls:

- `all(output1, output2[, ...])`: create a new output that copies its' input to all of the provided outputs.
- `any(button1, button2[, ...])`: create a virtual button that is pressed when any of the specified buttons are pressed, and released when all are released. If multiple buttons are pressed, it will only emit 1 pressed event. This can be useful if a hat is exposed as multiple buttons, and you want to treat it as a single button.

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
AxisToHat myHat(vjoy1.Hat1, 90_percent); // custom deadzone
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

## AxisTrimmer

Trim (recenter) an axis by pressing and holding a button.

Any movement is ignored while the button is pressed; trim is adjusted by the
position difference between where the button was pressed and where it was
released.

For example, if you move the joystick to (100, 100), press the button, center
the stick, then release the button, when centered, the joystick will report
(100, 100); if you then move to (100, 100), it will report (200, 200).

- a separate trimmer is needed for each axis
- trim is limited to half of the axis range
- a reset button is also supported

It is used like this:

```c++
AxisTrimmer xtrim, ytrim;
cyclic.XAxis >> &xtrim >> vj.XAxis;
cyclic.YAxis >> &ytrim >> vj.YAxis;

cyclic.Button21 >> all(xtrim.TrimButton, ytrim.TrimButton);
cyclic.Button19 >> all(xtrim.ResetButton, ytrim.ResetButton);
```

## ButtonToAxis

This converts a button to an axis; this axis is always either 0 (if the button is not pressed), or at the maximum value (if the button is pressed). This can be useful for binding buttons to X360 controller triggers, which are axis. For example:

```C++
stick.Button1 >> ButtonToAxis() >> x360.RTrigger;
```

## HatToButtons

Some games treat hat directions as if they were buttons; this can be problematic if they support N/E/S/W bindings, but don't understand that a hat set to North-East should be treated as if the north + east buttons were pressed. In this situation, an 8-way hat only gives you 4 directions, but if you map it to buttons and bind them instead, you get 8-way control.

There are two ways to use this:

```C++
// Explicitly specify buttons:
stick.Hat1 >> HatToButtons(vj.Button1, vj.Button2, vj.Button3, vj.Button4);
// easy mode:
stick.Hat1 >> HatToButtons(
  &vj,
  stick.getButtonCount() + 1 // first button number
  4 // number of buttons
);
```

Additionally, a button can be pressed when the hat is centered:

```C++
stick.Hat1 >> HatToButtons(HatToButtons::CenterButton(vj.Button1), vj.Button2, vj.Button3, vj.Button4, vj.Button5);
stick.Hat1 >> HatToButtons(
  HatToButton::CenterButton(vj.button(stick.getButtonCount() + 1)),
  &vj,
  stick.getButtonCount() + 2 // first button number
  4 // number of buttons
);
```

Finally, interpolation can be disabled so that buttons are only pressed when the
hat is pointing *exactly* at them - for example, with an 8 way hat with 4 buttons,
NE on the hat will not trigger N or E buttons:

```C++
stick.Hat1 >> HatToButtons(
  HatToButtons::Interpolation::None,
  vj.Button2,
  vj.Button3,
  vj.Button4,
  vj.Button5);
```

## LatchedToMomentaryButton

Some controllers have buttons that 'latch' or maintain their state, such as toggle switches that stay in position, or
flip-up safeties/triggers that are reported as a continuously held button.

Some games require a toggle on/off input instead of a continuous press - this allows that conversion.

```C++
stick.Button1 >> LatchedToMomentaryButton() >> vj.Button1;
```

## MomentaryToLatchedButton

Does the exact opposite of `LatchedToMomentaryButton`: it converts a short press into a toggle.

```C++
stick.Button1 >> MomentaryToLatchedButton() >> vj.Button1;
```

## Shift

Switches between actions, depending on an on/off state:

```C++
bool shifted = false;
stick.Button15 >> &shifted;
stick.Button16 >> Shift {
  &shifted,
  /* normal behavior */ vj.Button1,
  /* shifted behavior */ vj.Button2,
};
```

## ShortPressLongPress

This divides a button into two, depending on if a button is pressed and quickly released, or held for a moment.

This will always delay input, as when the button is pressed, we can't tell if it will be a short press or a long press - so, the output device (e.g. vjoy) will not indicate that a button has been pressed until the physical button has been released. The button release will be artificially delayed by 100ms, as some games will only respond to presses/releases that last multiple frames.

```C++
stick.Button1 >> ShortPressLongPress(vj.Button1, vj.Button2);
// Require a /really/ long press
stick.Button1 >> ShortPressLongPress(vj.Button1, vj.Button2, std::chrono::seconds(2));
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
must set a timer:

```C++
#include "Clock.h"
#include <chrono>

void MyAction::map(bool value) {
  Clock::get()->setTimer(
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

# Combining with programmable firmware (e.g. Virpil) or drivers (e.g. Thrustmaster TARGET)

It is up to you how much you want to do in vendor software vs in C++. I prefer to do as much as possible in C++ to:
- minimize the number of firmware reflashes
- avoid tieing my profiles to a particular vendor more than necessary

If you feel similarly, I recommend setting the vendor software/firmware to provide as much information as possible; if this causes problems with games, simplify it in C++. For example:
- set any flip-up triggers, safeties, or other latching buttons so that the 'real' button press indicates their state; for example, 'safety on' = pressed, 'safety off' = unpressed. If the game only supports 'toggle safety on/off' (e.g. the flip-up trigger in DCS Ka-50), use `LatchedToMomentaryButton` instead of changing the firwmware.
- set any hats either as true continuous (360-degree) hats, or 8-way hats.
	 - use `HatToButtons` if you have a discrete hat, but software requries buttons
	 - if the game requires a single button, use `any(HatButton1, HatButton2, ...) >> vjoy.Button123`
	 - if hats support 8-way, it is easy to accidentally get to an 'unpressed' state - for example, if you're pushing North East, this will report as unpressed. If set as an 8-way button-based hat, borth 'North' and 'East' buttons should be reported as pressed.

# Interacting with common utilities

If you use utilities (e.g. VoiceAttack, SRS, TeamSpeak) with multiple devices and multiple profiles/games, it can be helpful to remap to a consistent VJoy device and button for every profile where you want to use that functionality.

For example, you may want the same VJoy button for 'push to talk' on an A-10C throttle profile, and on a Ka-50 cyclic profile.

When picking a button, keep in mind that while most games and software support 32 joystick buttons, some supports 64, and some supports 128. For common utilities, it's generally best to select the highest button number that it supports - for example, if you are not currently using vjoy button 128, start with that, otherwise 127 etc. If 128 is otherwise unused but not recognized by your software, try 64 - and finally, button 32.

If you fly multiple aircraft in DCS World, this can also be useful for 'UI layer' features; for example, you might want to bind 'vjoy 1 button 128' to 'recenter VR headset' in all profiles.

## Debugging

### Compilation errors

I strongly recommend installing
[LLVM and Clang](https://github.com/llvm/llvm-project/releases/latest),
and running `build.ps1 -Compiler clang`; both Clang and cl (Microsoft Visual
C++'s compiler) are supported, however when there are issues, Clang gives
much more detailed error messages than cl.

### Runtime problems

- Build with `build.ps1 -BuildMode Debug` to get a more
  debuggable/less-optimized executable. If you are not using clang, ASAN will
  be enabled.
- Use your favorite debugger :) If you use VS Code, put your profile in profiles/,
  open it, and run "Current Profile (Debug)" from the "Run and Debug" tab on the
  left.


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
