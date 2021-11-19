/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "render_axis.h"

#include <fstream>

using std::ios;
using std::ofstream;

#pragma pack(push)
#pragma pack(1)
namespace {
struct Pixel {
  uint8_t b;
  uint8_t g;
  uint8_t r;
  // We don't use alpha, but it means we don't need to deal with 4-byte
  // padding at the end of each line
  uint8_t a = 0xff;
};
struct DIBHeader {
  uint32_t size = sizeof(DIBHeader);
  uint32_t width;
  uint32_t height;
  uint16_t color_planes = 1;
  uint16_t bits_per_pixel = 32;
  uint32_t format = 0;// BI_BITFIELD
  uint32_t data_size;
  uint32_t x_dpi = 72;
  uint32_t y_dpi = 72;
  uint32_t palette_colors = 0;
  uint32_t important_colors = 0;// all
};
struct BMPHeader {
  char magic[2] = {'B', 'M'};
  uint32_t file_size;
  uint16_t app_defined_1 = 0;
  uint16_t app_defined_2 = 0;
  uint32_t data_offset;
};
}// namespace
#pragma pack(pop)

namespace fredemmott::inputmapping {

void render_axis(
  const std::string& bmp_filename,
  AxisOutput& transform_in,
  AxisInput& transform_out) {
  long fx = -1;
  transform_out >> AxisOutput([&fx](long value) { fx = value; });
  transform_out >> [&fx](long value) { fx = value; };
  const int step = 128;
  // + 1 for 'max value' vs 'number of distinct values'
  const auto resolution = (0xffff + 1) / step;

  auto data = std::make_unique<Pixel[]>(resolution * resolution);
  // fill with white
  memset(data.get(), 0xff, resolution * resolution * sizeof(Pixel));

  // draw axis in black
  auto offset = [&](int x, int y) { return (x + (y * resolution)); };
  for (int i = 0; i < resolution; ++i) {
    data[offset(i, resolution / 2)] = {0, 0, 0};
    data[offset(resolution / 2, i)] = {0, 0, 0};
  }

  // finally, the values :)
  for (int i = 0; i <= 0xffff; ++i) {
    transform_in.map(i);
    const auto x = i / step;
    const auto y = fx / step;
    Pixel& p = data[offset(x, y)];
    // The more inputs hit this pixel, the bluer it gets
    p.r -= 0xff / step;
    p.g -= 0xff / step;
    p.b = 0xff;
  }

  // Now to convert raw pixel data to a BMP image
  DIBHeader dib;
  dib.width = dib.height = resolution;
  dib.data_size = resolution * resolution * sizeof(Pixel);
  BMPHeader bmp;
  bmp.file_size = sizeof(bmp) + sizeof(dib) + dib.data_size;
  bmp.data_offset = sizeof(bmp) + sizeof(dib);

  ofstream file;
  file.open(bmp_filename, ios::out | ios::trunc | ios::binary);
  file.write((const char*)&bmp, sizeof(bmp));
  file.write((const char*)&dib, sizeof(dib));
  file.write((const char*)data.get(), dib.data_size);
  file.close();
  printf("Wrote to %s\n", bmp_filename.data());
}

}// namespace fredemmott::inputmapping
