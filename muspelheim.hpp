#ifndef INC_MUSPELHEIM_HPP
#define INC_MUSPELHEIM_HPP

#include <iostream>

#include <functional>
#include <random>
#include <vector>

#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>

// Make sure that multiplying points by floating-point values works correctly.
namespace boost { namespace gil {

  template <typename T, typename U>
  point2<T> operator *(const point2<T> &p, U t) {
    return point2<T>(p.x*t, p.y*t);
  }
  template <typename T, typename U>
  point2<U> operator *(T t, const point2<U> &p) {
    return point2<U>(t*p.x, t*p.y);
  }

} }

namespace muspelheim {

template<typename Pixel, typename T = double>
class flame_function {
public:
  using point_type = boost::gil::point2<T>;
  using pixel_type = Pixel;
  using function_type = std::function<
    point_type(const point_type &, const affine_transform<T> &)
  >;
  using value_type = std::pair<function_type, T>;

  flame_function(const function_type &f,
                 const affine_transform<T> &transform,
                 const Pixel &color,
                 const affine_transform<T> &post = {1, 0, 0, 0, 1, 0})
    : flame_function({{f, 1}}, transform, color, post) {}

  flame_function(const std::initializer_list<value_type> &f,
                 const affine_transform<T> &transform,
                 const Pixel &color,
                 const affine_transform<T> &post = {1, 0, 0, 0, 1, 0})
    : f_(f), transform_(transform), color_(color), post_(post) {}

  inline point_type operator ()(const point_type &p) const {
    point_type value = {0, 0};
    const auto &transformed = transform_(p);
    for(const auto &i : f_)
      value += i.second * i.first(transformed, transform_);
    return post_(value);
  }

  inline pixel_type color() const {
    return color_;
  }
private:
  std::vector<value_type> f_;
  affine_transform<T> transform_;
  Pixel color_;
  affine_transform<T> post_;
};

template<typename Pixel, typename T = double>
using flame_function_set = std::vector<flame_function<Pixel, T>>;

template<typename ChannelValue, typename Layout>
boost::gil::pixel<ChannelValue, Layout>
blend(const boost::gil::pixel<ChannelValue, Layout> &a,
      const boost::gil::pixel<ChannelValue, Layout> &b, double ratio) {
  boost::gil::pixel<ChannelValue, Layout> p;
  constexpr size_t channels = boost::mpl::size<
    typename Layout::color_space_t
  >::value;

  for(int i = 0; i < channels; i++)
    p[i] = a[i] * ratio + b[i] * (1 - ratio);
  return p;
}

template<typename View, typename Pixel, typename T>
void chaos_game(const View &dst, const flame_function_set<Pixel, T> &funcs,
                size_t num_iterations = 10000000) {
  using namespace boost::gil;
  using biunit_pt = point2<T>;
  using image_pt = point2<ptrdiff_t>;
  using color_image_t = image<Pixel, false>;

  std::default_random_engine engine(std::random_device{}());
  std::uniform_int_distribution<size_t> random_func(0, funcs.size() - 1);
  std::uniform_real_distribution<T> random_biunit(-1, 1);

  gray32_image_t alpha_img(dst.dimensions(), gray32_pixel_t(0), 0);
  auto alpha = view(alpha_img);
  bits32 max_alpha = 0;

  color_image_t color_img(dst.dimensions(), Pixel(0), 0);
  auto color = view(color_img);

  biunit_pt point(random_biunit(engine), random_biunit(engine));

  for(size_t i = 0; i < 20; i++)
    point = funcs[random_func(engine)](point);

  for(size_t i = 0; i < num_iterations; i++) {
    auto &f = funcs[random_func(engine)];
    point = f(point);
    image_pt pt(
      static_cast<ptrdiff_t>((point.x + 1) / 2 * alpha.width()),
      static_cast<ptrdiff_t>((point.y + 1) / 2 * alpha.height())
    );
    if(pt.x < 0 || pt.x >= alpha.width() ||
       pt.y < 0 || pt.y >= alpha.height())
      continue;

    if(!alpha(pt)[0])
      color(pt) = f.color();
    else
      color(pt) = blend(color(pt), f.color(), 0.9);

    alpha(pt)[0]++;
    if(alpha(pt)[0] > max_alpha)
      max_alpha = alpha(pt)[0];
  }

  auto logmax = std::log(static_cast<T>(max_alpha));
  for(size_t x = 0; x < dst.width(); x++) {
    for(size_t y = 0; y < dst.height(); y++) {
      auto a = std::log(static_cast<T>(alpha(x, y)[0])) / logmax;
      auto &c = color(x, y);
      dst(x, y) = typename View::value_type(c[0] * a, c[1] * a, c[2] * a);
    }
  }
}

} // namespace muspelheim

#endif
