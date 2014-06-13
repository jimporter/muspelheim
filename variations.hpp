#ifndef INC_VARIATIONS_HPP
#define INC_VARIATIONS_HPP

#include <cmath>

#include <boost/gil/utilities.hpp>

namespace muspelheim {

template<typename T = double>
class affine_transform {
public:
  affine_transform(T a, T b, T c, T d, T e, T f)
    : a(a), b(b), c(c), d(d), e(e), f(f) {}

  boost::gil::point2<T> operator ()(const boost::gil::point2<T> &p) const {
    return boost::gil::point2<T>(a*p.x + b*p.y + c, d*p.x + e*p.y + f);
  }
private:
  T a, b, c, d, e, f;
};

template<typename T = double>
class linear {
public:
  linear(const affine_transform<T> &t) : transform(t) {}

  boost::gil::point2<T> operator ()(const boost::gil::point2<T> &p) const {
    return transform(p);
  }
private:
  affine_transform<T> transform;
};

template<typename T = double>
class sinusoidal {
public:
  sinusoidal(const affine_transform<T> &t) : transform(t) {}

  boost::gil::point2<T> operator ()(const boost::gil::point2<T> &p) const {
    auto result = transform(p);
    return boost::gil::point2<T>(std::sin(result.x), std::cos(result.y));
  }
private:
  affine_transform<T> transform;
};


template<typename T = double>
class spherical {
public:
  spherical(const affine_transform<T> &t) : transform(t) {}

  boost::gil::point2<T> operator ()(const boost::gil::point2<T> &p) const {
    boost::gil::point2<T> result = transform(p);
    result /= (result.x*result.x + result.y*result.y);
    return result;
  }
private:
  affine_transform<T> transform;
};

template<typename T = double>
class handkerchief {
public:
  handkerchief(const affine_transform<T> &t) : transform(t) {}

  boost::gil::point2<T> operator ()(const boost::gil::point2<T> &p) const {
    boost::gil::point2<T> result = transform(p);
    T r = std::sqrt(result.x*result.x + result.y*result.y);
    T theta = std::atan(result.x/result.y);
    return boost::gil::point2<T>(r*std::sin(theta+r), r*std::cos(theta-r));
  }
private:
  affine_transform<T> transform;
};

template<typename T = double>
class spiral {
public:
  spiral(const affine_transform<T> &t) : transform(t) {}

  boost::gil::point2<T> operator ()(const boost::gil::point2<T> &p) const {
    boost::gil::point2<T> result = transform(p);
    T r = std::sqrt(result.x*result.x + result.y*result.y);
    T theta = std::atan(result.x/result.y);
    return boost::gil::point2<T>(
      (std::cos(theta) + std::sin(r)) / r,
      (std::sin(theta) - std::cos(r)) / r
    );
  }
private:
  affine_transform<T> transform;
};

} // namespace muspelheim

#endif
