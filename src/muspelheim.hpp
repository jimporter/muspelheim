#ifndef INC_MUSPELHEIM_HPP
#define INC_MUSPELHEIM_HPP

#include <functional>
#include <random>
#include <vector>

#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>

#include "transform.hpp"

namespace muspelheim {

template<typename Pixel>
class flame_function {
public:
  using pixel_type = Pixel;
  using function_type = std::function<
    vec2d(const vec2d &, const affine_transform &)
  >;
  using value_type = std::pair<function_type, double>;

  flame_function(const function_type &f,
                 const affine_transform &transform,
                 const Pixel &color,
                 const affine_transform &post = identity())
    : flame_function({{f, 1}}, transform, color, post) {}

  flame_function(const std::initializer_list<value_type> &f,
                 const affine_transform &transform,
                 const Pixel &color,
                 const affine_transform &post = identity())
    : f_(f), transform_(transform), color_(color), post_(post) {}

  inline vec2d operator ()(const vec2d &p) const {
    vec2d value = {0, 0};
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
  affine_transform transform_;
  pixel_type color_;
  affine_transform post_;
};

template<typename Pixel>
using flame_function_set = std::vector<flame_function<Pixel>>;

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

template<typename ColorPixel, typename AlphaPixel>
struct image_data {
  using color_pixel = ColorPixel;
  using alpha_pixel = AlphaPixel;

  template<typename Color, typename Alpha>
  image_data(Color &&color, Alpha &&alpha) :
    color(std::forward<Color>(color)),
    alpha(std::forward<Alpha>(alpha)) {}

  explicit image_data(const boost::gil::point2<ptrdiff_t> &dimensions) :
    color(dimensions, color_pixel(0), 0),
    alpha(dimensions, alpha_pixel(0), 0) {}

  boost::gil::image<color_pixel, false> color;
  boost::gil::image<alpha_pixel, false> alpha;
};

template<typename ColorPixel>
using raw_image_data = image_data<ColorPixel, uint32_t>;
template<typename ColorPixel>
using cooked_image_data = image_data<ColorPixel, double>;

template<typename Pixel>
raw_image_data<Pixel>
chaos_game(const flame_function_set<Pixel> &funcs,
           const boost::gil::point2<ptrdiff_t> &dimensions,
           size_t num_iterations = 10000000) {
  using namespace boost::gil;
  using image_pt = point2<ptrdiff_t>;

  std::default_random_engine engine(std::random_device{}());
  std::uniform_int_distribution<size_t> random_func(0, funcs.size() - 1);
  std::uniform_real_distribution<double> random_biunit(-1, 1);

  raw_image_data<Pixel> result(dimensions);
  auto color = view(result.color);
  auto alpha = view(result.alpha);

  vec2d point(random_biunit(engine), random_biunit(engine));

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

    if(!alpha(pt))
      color(pt) = f.color();
    else
      color(pt) = blend(color(pt), f.color(), 0.9);

    alpha(pt)++;
  }

  return result;
}

namespace detail {
  inline auto do_linear_alpha(const boost::gil::image<uint32_t, false> &src) {
    using namespace boost::gil;

    image<double, false> dst(src.dimensions(), 0, 0);

    auto src_view = const_view(src);
    auto dst_view = view(dst);

    auto max_alpha = *std::max_element(src_view.begin(), src_view.end());
    for(size_t i = 0; i != dst_view.size(); i++)
      dst_view[i] = static_cast<double>(src_view[i]) / max_alpha;
    return dst;
  }

  inline auto do_log_alpha(const boost::gil::image<uint32_t, false> &src) {
    using namespace boost::gil;

    image<double, false> dst(src.dimensions(), 0, 0);

    auto src_view = const_view(src);
    auto dst_view = view(dst);

    auto max_alpha = *std::max_element(src_view.begin(), src_view.end());
    auto logmax = std::log(static_cast<double>(max_alpha));
    for(size_t i = 0; i != dst_view.size(); i++)
      dst_view[i] = std::log(static_cast<double>(src_view[i])) / logmax;
    return dst;
  }
}

template<typename ColorPixel>
cooked_image_data<ColorPixel>
linear_alpha(const raw_image_data<ColorPixel> &src) {
  return {src.color, detail::do_linear_alpha(src.alpha)};
}

template<typename ColorPixel>
cooked_image_data<ColorPixel>
linear_alpha(raw_image_data<ColorPixel> &&src) {
  return {std::move(src.color), detail::do_linear_alpha(src.alpha)};
}

template<typename ColorPixel>
cooked_image_data<ColorPixel>
log_alpha(const raw_image_data<ColorPixel> &src) {
  return {src.color, detail::do_log_alpha(src.alpha)};
}

template<typename ColorPixel>
cooked_image_data<ColorPixel>
log_alpha(raw_image_data<ColorPixel> &&src) {
  return {std::move(src.color), detail::do_log_alpha(src.alpha)};
}

template<typename View, typename ColorPixel>
void render(View &dst, const cooked_image_data<ColorPixel> &src,
            double gamma = 1.0) {
  using namespace boost::gil;

  auto color = const_view(src.color);
  auto alpha = const_view(src.alpha);

  double inv_gamma = 1/gamma;
  for(size_t i = 0; i != dst.size(); i++) {
    assert(alpha[i] <= 1);
    auto a = std::pow(alpha[i], inv_gamma);
    auto &c = color[i];
    dst[i] = typename View::value_type(c[0] * a, c[1] * a, c[2] * a);
  }
}

} // namespace muspelheim

#endif
