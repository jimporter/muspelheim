#ifndef INC_TRANSFORM_HPP
#define INC_TRANSFORM_HPP

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

  affine_transform & operator *=(const affine_transform &rhs) {
    *this = *this * rhs;
    return *this;
  }

  affine_transform
  operator +(const affine_transform &rhs) {
    return affine_transform(*this) += rhs;
  }

  affine_transform
  operator -(const affine_transform &rhs) {
    return affine_transform(*this) -= rhs;
  }

  affine_transform
  operator *(const affine_transform &rhs) {
    return affine_transform(
      a*rhs.a + b*rhs.d, a*rhs.b + b*rhs.e, a*rhs.c + b*rhs.f + c,
      d*rhs.a + e*rhs.d, d*rhs.b + e*rhs.e, d*rhs.c + e*rhs.f + f
    );
  }

  friend
  std::ostream & operator <<(std::ostream &os, const affine_transform &t) {
    return os << "{ " << t.a << ", " << t.b << ", " << t.c
              << ", " << t.d << ", " << t.e << ", " << t.f << " }" << std::endl;
  }
private:
  T a, b, c, d, e, f;
};

template<typename T>
affine_transform<T> identity() {
  return { 1, 0, 0,  0, 1, 0 };
}

template<typename T>
affine_transform<T> scale(T factor) {
  return { factor, 0, 0,  0, factor, 0 };
}

template<typename T>
affine_transform<T> rotate(T theta) {
  return { std::cos(theta), -std::sin(theta), 0,
           std::sin(theta),  std::cos(theta), 0 };
}

template<typename T>
affine_transform<T> translate(T x, T y) {
  return { 1, 0, x,  0, 1, y };
}

} // namespace muspelheim

#endif
