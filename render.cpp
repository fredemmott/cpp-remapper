/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

// This file is an executable, intended to generate illustrative diagrams
// of some basic axis mapping operations

#include "lib/easymode.h"
#include "lib/render_axis.h"
#include "lib/axiscurve.h"

using fredemmott::inputmapping::render_axis;
using fredemmott::inputmapping::UnsafeRef;
using fredemmott::inputmapping::Sink;
using fredemmott::inputmapping::Source;

/*
template<typename TSink, typename TSource>
class PipelineTransform : public Sink<typename TSink::In>, public Source<typename TSink::Out> {
  UnsafeRef<TSink> mFirst;
  public:
    PipelineTransform(UnsafeRef<TSink> first, UnsafeRef<TSource> last): mFirst(first) {
      ::fredemmott::inputmapping::PipelineHead(last).bind([this](typename TSource::Out){ emit(value()); });
    }
    virtual void map(typename TSink::In value) override {
      mFirst->map(value);
    }
};

template<
  typename TSink,
  typename TSource,
  std::enable_if_t<std::is_base_of_v<::fredemmott::inputmapping::SourceBase, TSource>, bool> = true,
  std::enable_if_t<std::is_base_of_v<::fredemmott::inputmapping::SourceBase, TSink>, bool> = true,
  std::enable_if_t<std::is_base_of_v<::fredemmott::inputmapping::SinkBase, TSink>, bool> = true
>
auto operator>>(TSink&& lhs, TSource&& rhs) {
  UnsafeRef left = UnsafeRef(std::make_shared<TSink>(std::move(lhs)));
  UnsafeRef right = UnsafeRef(std::make_shared<TSource>(std::move(rhs)));
  left->setNext(right);
  return PipelineTransform(left, right);
}
*/

int main() {
  render_axis("linear.bmp", [](auto next) { return next; });
  render_axis("curve_0.bmp", AxisCurve(0));
  render_axis("curve_0.5.bmp", AxisCurve(0.5));
  render_axis("curve_neg0.5.bmp",AxisCurve(-0.5));
  render_axis("curve_0.99.bmp", AxisCurve(0.99));
  render_axis("dead_10.bmp", SquareDeadzone(10));
  /*
   render_axis("dead_curve.bmp", SquareDeadzone(50) >> AxisCurve(0.5));
   render_axis("curve_dead.bmp", AxisCurve(0.5) >> SquareDeadzone(50));
   */
  return 0;
}
