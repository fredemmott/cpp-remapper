#include "easymode.h"

int main() {
  auto [p, throttle, stick, vj1, vj2] = create_profile(
    TM_WARTHOG_THROTTLE,
    VPC_RIGHT_WARBRD,
    VJOY_1,
    VJOY_2
  );

  p->passthrough(throttle, vj1);
  p->passthrough(stick, vj2);

  // Replace the ministick mappings on both devices to additional buttons
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
