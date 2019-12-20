#ifndef INC_MUSPELHEIM_VARIATIONS_HPP
#define INC_MUSPELHEIM_VARIATIONS_HPP

#include <cmath>

#include "vec2d.hpp"

namespace math {

  inline vec2d linear(const vec2d &p, const affine_transform &) {
    return p;
  }

  inline vec2d sinusoidal(const vec2d &p, const affine_transform &) {
    return {std::sin(p.x), std::cos(p.y)};
  }

  inline vec2d spherical(const vec2d &p, const affine_transform &) {
    return p / (p.x*p.x + p.y*p.y);
  }

  inline vec2d swirl(const vec2d &p, const affine_transform &) {
    auto r2 = p.x*p.x + p.y*p.y;
    return {
      p.x * std::sin(r2) - p.y * cos(r2),
      p.x * std::sin(r2) + p.y * cos(r2)
    };
  }

  inline vec2d handkerchief(const vec2d &p, const affine_transform &) {
    const auto &r = std::hypot(p.x, p.y);
    const auto &theta = std::atan(p.x / p.y);
    return {
      r * std::sin(theta + r),
      r * std::cos(theta - r)
    };
  }

  inline vec2d spiral(const vec2d &p, const affine_transform &) {
    const auto &r = std::hypot(p.x, p.y);
    const auto &theta = std::atan(p.x / p.y);
    return {
      (std::cos(theta) + std::sin(r)) / r,
      (std::sin(theta) - std::cos(r)) / r
    };
  }

} // namespace math

#endif
