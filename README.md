# What is this?

This is a very early stage input device remapper, for mapping code (profiles)
written in C++.

It aims to:
- use modern standard C++ wherever possible
- have minimum dependencies

It uses vJoy, and supports HidGuardian.

# What does a profile look like?

``` C++
const auto first_free_button = throttle.getButtonCount() + 1;
mapper.map(
  { // axes
    { throttle.ZAxis, vjoy1.RXAxis  },
    {
      throttle.XAxis,
      AxisToButtons {
        {0, 0, vjoy1.button(first_free_button) },
        {100, 100, vjoy1.button(first_free_button + 1) }
      }
    },
    {
      throttle.YAxis,
      AxisToButtons {
        {0, 0, vjoy1.button(first_free_button + 2) },
        {100, 100, vjoy1.button(first_free_button + 3) }
      }
    }
  }, { // hats
  }, { // buttons
    { throttle.button(1), vjoy1.button(1) },
  },
);
```

It also supports `passthrough(throttle, vjoy1);` to copy all inputs to
outputs; later calls to `map()` will only replace specified outputs, not
the entire map.

# How do I use this?

You probably shouldn't yet :)

That said:

1. Download the vJoy SDK - the version must exactly match the version of vJoy
   you're using.
2. Extract it to the source directory so you have an `SDK/` subdirectory
3. Install Visual Studio 2019 or newer
4. Open visual studio command prompt
5. `cl /Fetest.exe /EHsc /std:c++17 *.cpp`
6. Run `test.exe`
