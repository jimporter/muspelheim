#ifndef INC_VARIATIONS_HPP
#define INC_VARIATIONS_HPP

#include <cmath>

#include "transform.hpp"

namespace muspelheim {

template<typename T = double>
struct linear {
  using point_type = boost::gil::point2<T>;

  point_type
  operator ()(const point_type &p, const affine_transform<T> &) const {
    return p;
  }
};

template<typename T = double>
struct sinusoidal {
  using point_type = boost::gil::point2<T>;

  point_type
  operator ()(const point_type &p, const affine_transform<T> &) const {
    return point_type(std::sin(p.x), std::cos(p.y));
  }
};

template<typename T = double>
struct spherical {
  using point_type = boost::gil::point2<T>;

  point_type
  operator ()(const point_type &p, const affine_transform<T> &) const {
    return p / (p.x*p.x + p.y*p.y);
  }
};

template<typename T = double>
struct swirl {
  using point_type = boost::gil::point2<T>;

  point_type
  operator ()(const point_type &p, const affine_transform<T> &) const {
    auto r2 = p.x*p.x + p.y*p.y;
    return point_type(
      p.x * std::sin(r2) - p.y * cos(r2),
      p.x * std::sin(r2) + p.y * cos(r2)
    );
  }
};

template<typename T = double>
struct handkerchief {
  using point_type = boost::gil::point2<T>;

  point_type
  operator ()(const point_type &p, const affine_transform<T> &) const {
    const auto &r = std::hypot(p.x, p.y);
    const auto &theta = std::atan(p.x / p.y);
    return point_type(
      r * std::sin(theta + r),
      r * std::cos(theta - r)
    );
  }
};

template<typename T = double>
struct spiral {
  using point_type = boost::gil::point2<T>;

  point_type
  operator ()(const point_type &p, const affine_transform<T> &) const {
    const auto &r = std::hypot(p.x, p.y);
    const auto &theta = std::atan(p.x / p.y);
    return point_type(
      (std::cos(theta) + std::sin(r)) / r,
      (std::sin(theta) - std::cos(r)) / r
    );
  }
};

} // namespace muspelheim

#endif
