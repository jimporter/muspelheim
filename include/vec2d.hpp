#ifndef INC_MUSPELHEIM_VEC2D_HPP
#define INC_MUSPELHEIM_VEC2D_HPP

#include <cmath>

namespace math {

struct vec2d {
  vec2d() = default;
  vec2d(double x, double y) : x(x), y(y) {}

  vec2d & operator +=(const vec2d &rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  vec2d & operator -=(const vec2d &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  vec2d & operator *=(double rhs) {
    x *= rhs;
    y *= rhs;
    return *this;
  }

  vec2d & operator /=(double rhs) {
    x /= rhs;
    y /= rhs;
    return *this;
  }

  friend vec2d operator +(const vec2d &x, const vec2d &y) {
    return vec2d(x) += y;
  }

  friend vec2d operator -(const vec2d &x, const vec2d &y) {
    return vec2d(x) -= y;
  }

  friend vec2d operator *(const vec2d &x, double &y) {
    return vec2d(x) *= y;
  }

  friend vec2d operator *(double x, const vec2d &y) {
    return vec2d(y) *= x;
  }

  friend vec2d operator /(const vec2d &x, double y) {
    return vec2d(x) /= y;
  }

  double x, y;
};

struct affine_transform {
  affine_transform() = default;
  affine_transform(double a, double b, double c, double d, double e, double f) :
    a(a), b(b), c(c), d(d), e(e), f(f) {}

  vec2d operator ()(const vec2d &p) const {
    return { a*p.x + b*p.y + c, d*p.x + e*p.y + f };
  }

  affine_transform & operator +=(const affine_transform &rhs) {
    a += rhs.a; b += rhs.b; c += rhs.c;
    d += rhs.d; e += rhs.e; f += rhs.f;
    return *this;
  }

  affine_transform & operator -=(const affine_transform &rhs) {
    a -= rhs.a; b -= rhs.b; c -= rhs.c;
    d -= rhs.d; e -= rhs.e; f -= rhs.f;
    return *this;
  }

  affine_transform & operator *=(double rhs) {
    a *= rhs; b *= rhs; c *= rhs;
    d *= rhs; e *= rhs; f *= rhs;
    return *this;
  }

  affine_transform & operator /=(double rhs) {
    a /= rhs; b /= rhs; c /= rhs;
    d /= rhs; e /= rhs; f /= rhs;
    return *this;
  }

  friend affine_transform
  operator +(const affine_transform &x, const affine_transform &y) {
    return affine_transform(x) += y;
  }

  friend affine_transform
  operator -(const affine_transform &x, const affine_transform &y) {
    return affine_transform(x) -= y;
  }

  friend affine_transform
  operator *(const affine_transform &x, double &y) {
    return affine_transform(x) *= y;
  }

  friend affine_transform
  operator *(double x, const affine_transform &y) {
    return affine_transform(y) *= x;
  }

  friend affine_transform
  operator /(const affine_transform &x, double &y) {
    return affine_transform(x) /= y;
  }

  friend affine_transform
  operator *(const affine_transform &x, const affine_transform &y) {
    return {
      x.a*y.a + x.b*y.d, x.a*y.b + x.b*y.e, x.a*y.c + x.b*y.f + x.c,
        x.d*y.a + x.e*y.d, x.d*y.b + x.e*y.e, x.d*y.c + x.e*y.f + x.f
        };
  }

  affine_transform & operator *=(const affine_transform &rhs) {
    *this = *this * rhs;
    return *this;
  }

  double a, b, c, d, e, f;
};

inline affine_transform identity() {
  return { 1, 0, 0,  0, 1, 0 };
}

inline affine_transform scale(double factor) {
  return { factor, 0, 0,  0, factor, 0 };
}

inline affine_transform rotate(double theta) {
  return { std::cos(theta), -std::sin(theta), 0,
           std::sin(theta),  std::cos(theta), 0 };
}

inline affine_transform translate(double x, double y) {
  return { 1, 0, x,  0, 1, y };
}

} // namespace math

#endif
