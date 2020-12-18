#include "easymode.h"

// This file is pretty much expanding what "it compiles, ship it!"
// covers

namespace fredemmott::inputmapping::apicompilertest {

void test_template_conversions() {
  Mapper m;
  MappableInput i(nullptr);
  MappableOutput o(1);

  // Basic passthrough
  //   Impl: (Axis|Button|Hat)Target -> VJoy(Axis|Button|Hat)
  m.map(i.XAxis, o.XAxis);
  m.map(i.Button1, o.Button1);
  m.map(i.Hat1, o.Hat1);

  // 1:n
  //   Impl: std::initializer_list<EventHandler> -> ComboTarget
  m.map(i.XAxis, {o.XAxis, o.YAxis});
  m.map(i.Button1, {o.Button1, o.Button2});
  m.map(i.Hat1, {o.Hat1, o.Hat2});

  // Lambdas
  //   Impl: T -> std::function<void(TValue)> -> FunctionAction
  m.map(i.XAxis, [=](long v) { o.XAxis.set(v); });
  m.map(i.Button1, [=](bool v) { o.Button1.set(v); });
  m.map(i.Hat1, [=](int16_t v) { o.Hat1.set(v); });

  // Here onwards, assuming that axis/button/hats are abstracted
  // adequately, so we don't need to test them all

  // Explicit actions
  m.map(i.Button1, VJoyButton(o.Button1));

  // 1:n lambdas
  m.map(
    i.Button1, 
    {
      [=](bool v) { o.Button1.set(v); },
      [=](bool v) { o.Button2.set(v); },
    }
  );

  // Multiple bindings at the same time
  m.map({
    { i.Button1, o.Button1 },
    { i.Button2, { o.Button2, o.Button3 } },
    { i.Button3, [](bool){} },
    { i.Button3, {[](bool){}, [](bool){} } },
  });
}

};
