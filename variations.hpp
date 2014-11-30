#ifndef INC_VARIATIONS_HPP
#define INC_VARIATIONS_HPP

#include <cmath>

#include <boost/gil/utilities.hpp>

namespace muspelheim {

template<typename T = double>
class affine_transform {
public:
  using point_type = boost::gil::point2<T>;

  affine_transform(T a, T b, T c, T d, T e, T f)
    : a(a), b(b), c(c), d(d), e(e), f(f) {}

  point_type operator ()(const point_type &p) const {
    return point_type(a*p.x + b*p.y + c, d*p.x + e*p.y + f);
  }
private:
  T a, b, c, d, e, f;
};

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
