#ifndef INC_MUSPELHEIM_HPP
#define INC_MUSPELHEIM_HPP

#include <iostream>

#include <functional>
#include <random>
#include <vector>

#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>

namespace muspelheim {

template<typename Pixel, typename T = double>
class flame_function {
public:
  using point_type = boost::gil::point2<T>;
  using pixel_type = Pixel;
  using function_type = std::function<point_type(const point_type &)>;

  flame_function(const function_type &f, const Pixel &color)
    : f_(f), color_(color) {}

  inline point_type operator ()(const point_type &p) const {
    return f_(p);
  }

  inline pixel_type color() const {
    return color_;
  }
private:
  function_type f_;
  Pixel color_;
};

template<typename Pixel, typename T = double>
using flame_function_set = std::vector<flame_function<Pixel, T>>;

template<typename View, typename Pixel, typename T>
void chaos_game(const View &dst, const flame_function_set<Pixel, T> &funcs,
                size_t num_iterations = 10000000) {
  using namespace boost::gil;
  using biunit_pt = point2<T>;
  using image_pt = point2<ptrdiff_t>;
  using color_image_t = image<Pixel, false>;

  std::mt19937 engine;
  std::uniform_int_distribution<size_t> random_func(0, funcs.size() - 1);
  std::uniform_real_distribution<T> random_biunit(-1, 1);

  gray32_image_t alpha_img(dst.dimensions(), gray32_pixel_t(0), 0);
  gray32_image_t::view_t alpha = view(alpha_img);
  bits32 max_alpha = 0;

  color_image_t color_img(dst.dimensions(), Pixel(0), 0);
  typename color_image_t::view_t color = view(color_img);

  biunit_pt point(random_biunit(engine), random_biunit(engine));

  for(size_t i = 0; i < 20; i++)
    point = funcs[random_func(engine)](point);

  for(size_t i = 0; i < num_iterations; i++) {
    auto &f = funcs[random_func(engine)];
    point = f(point);
    if(point.x >= -1 && point.x < 1 && point.y >= -1 && point.y < 1) {
      image_pt pt(
        static_cast<ptrdiff_t>((point.x + 1) / 2 * alpha.width()),
        static_cast<ptrdiff_t>((point.y + 1) / 2 * alpha.height())
      );
      alpha(pt)[0]++;
      if(alpha(pt)[0] > max_alpha)
        max_alpha = alpha(pt)[0];

      auto &a = color(pt);
      auto b = f.color();
      color(pt) = Pixel(
        a[0] + (b[0] - a[0]) / 2,
        a[1] + (b[1] - a[1]) / 2,
        a[2] + (b[2] - a[2]) / 2
      );
    }
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
