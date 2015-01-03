#include "colors.hpp"

#include <algorithm>
#include <cmath>

namespace colors {

hsv tohsv(const rgb &value) {
  auto minmax = std::minmax<uint32_t>({value.r, value.g, value.b});
  int delta = minmax.second - minmax.first;

  int hue;
  int r = value.r, g = value.g, b = value.b;
  if(minmax.second == value.r)
    hue = 60 * (g - b) / delta;
  else if(minmax.second == value.g)
    hue = 120 + 60 * (b - r) / delta;
  else // if(minmax.second == value.b)
    hue = 240 + 60 * (r - g) / delta;

  if(hue < 0)
    hue += 360;

  return hsv(hue, delta * 100 / minmax.second, minmax.second * 100 / 255);
}

rgb torgb(const hsv &value) {
  uint8_t c = 255 * value.s * value.v / (100*100);
  float hprime = value.h / 60.0f;
  uint8_t x = c * (1 - std::abs(std::fmod(hprime, 2) - 1));
  uint8_t m = 255 * value.v / 100 - c;

  switch(static_cast<int>(hprime)) {
  case 0:
    return rgb(c + m, x + m, 0 + m);
  case 1:
    return rgb(x + m, c + m, 0 + m);
  case 2:
    return rgb(0 + m, c + m, x + m);
  case 3:
    return rgb(0 + m, x + m, c + m);
  case 4:
    return rgb(x + m, 0 + m, c + m);
  case 5:
    return rgb(c + m, 0 + m, x + m);
  default:
    assert(false && "something bad happened");
  }
}

void color_theme_iterator::evaluate() {
  int dir = index_ % 2 == 0 ? 1 : -1;
  int offset = spread_ * (index_ >> 1);
  hsv value = base_;

  value.h = (value.h + dir * offset) % 360;
  if(value.h < 0)
    value.h += 360;
  curr_ = torgb(value);
}

} // namespace colors
