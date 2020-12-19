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

  // 1 physical button to 2 virtual buttons
  p->map(throttle.Button1, {vj1.Button1, vj1.Button2} );

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

# What helpers are available?

For now, only `AxisToButton`; you have two options to do more: defining an action, or using
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

# How do I use this?

1. Download the vJoy SDK - the version must exactly match the version of vJoy
   you're using. As of 2020-12-18, the most recent version appears to be
   available from https://github.com/njz3/vJoy/
2. Extract it to the source directory so you have an `SDK/` subdirectory
3. Copy the `vJoyInterface.dll` from the SDK directory to the repo root directory
4. Install Visual Studio 2019 or newer
5. Open visual studio command prompt
6. `cl /Fetest.exe /EHsc /std:c++17 /vmg *.cpp`
   - `/Fetest.exe`: create 'test.exe'
   - `/EHsc`: enable standard C++ exception/stack unwinding, with no exceptions
     from `extern "C"`
   - `/std:c++17`: enable C++17
   - `/vmg`: be pessimistic about member functions pointers to forward-declared
     classes. Prevents memory corruption in these cases.
6. Run `test.exe`; run as administrator for HidGuardian support.
