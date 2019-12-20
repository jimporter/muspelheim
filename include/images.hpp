#ifndef INC_MUSPELHEIM_IMAGE_HPP
#define INC_MUSPELHEIM_IMAGE_HPP

#include <boost/gil/image.hpp>

namespace images {

  template<typename Pixel>
  Pixel blend(const Pixel &a, const Pixel &b, double ratio) {
    Pixel p;
    for(size_t i = 0; i != boost::gil::size<Pixel>::value; i++)
      p[i] = a[i] * ratio + b[i] * (1 - ratio);
    return p;
  }

  template<typename ColorPixel, typename AlphaPixel>
  struct image_data {
    using color_pixel = ColorPixel;
    using color_image = boost::gil::image<color_pixel, false>;

    using alpha_pixel = AlphaPixel;
    using alpha_image = boost::gil::image<alpha_pixel, false>;

    template<typename Color, typename Alpha>
    image_data(Color &&color, Alpha &&alpha) :
      color(std::forward<Color>(color)),
      alpha(std::forward<Alpha>(alpha)) {}

    explicit image_data(const boost::gil::point2<ptrdiff_t> &dimensions) :
      color(dimensions, color_pixel(0), 0),
      alpha(dimensions, alpha_pixel(0), 0) {}

    auto dimensions() const {
      return color.dimensions();
    }

    color_image color;
    alpha_image alpha;
  };

  template<typename ColorPixel>
  using raw_image_data = image_data<ColorPixel, uint32_t>;
  template<typename ColorPixel>
  using cooked_image_data = image_data<ColorPixel, double>;

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

  template<typename View, typename ColorPixel>
  void render_monochrome(View &dst, const cooked_image_data<ColorPixel> &src,
                         const ColorPixel &color, double gamma = 1.0) {
    using namespace boost::gil;
    auto alpha = const_view(src.alpha);

    double inv_gamma = 1/gamma;
    for(size_t i = 0; i != dst.size(); i++) {
      assert(alpha[i] <= 1);
      auto a = std::pow(alpha[i], inv_gamma);
      for(size_t chan = 0; chan != boost::gil::size<ColorPixel>::value; chan++)
        dst[i][chan] = a * color[chan];
    }
  }

  template<typename Pixel>
  raw_image_data<Pixel>
  combine(const std::vector<raw_image_data<Pixel>> &srcs) {
    using namespace boost::gil;
    using image_data = raw_image_data<Pixel>;

    assert(!srcs.empty());
    image_data dst(srcs[0].dimensions());
    auto dst_color_view = view(dst.color);
    auto dst_alpha_view = view(dst.alpha);

    std::vector<typename image_data::color_image::const_view_t> src_color_views;
    std::vector<typename image_data::alpha_image::const_view_t> src_alpha_views;
    for(const auto &src : srcs) {
      src_color_views.push_back(const_view(src.color));
      src_alpha_views.push_back(const_view(src.alpha));
    }

    for(size_t px = 0; px != dst_color_view.size(); px++) {
      for(const auto &alpha : src_alpha_views)
        dst_alpha_view[px] += alpha[px];

      for(size_t s = 0; s != srcs.size(); s++) {
        double weight = static_cast<double>(src_alpha_views[s][px]) /
          dst_alpha_view[px];
        for(size_t chan = 0; chan != boost::gil::size<Pixel>::value; chan++)
          dst_color_view[px][chan] += src_color_views[s][px][chan] * weight;
      }
    }

    return dst;
  }

  template<typename View, typename ConstView>
  void lighten(View &a, const ConstView &b) {
    using namespace boost::gil;
    using dst_pixel = typename View::value_type;
    using channel_t = typename channel_type<dst_pixel>::type;
    auto max = channel_traits<channel_t>::max_value();

    for(size_t i = 0; i != a.size(); i++) {
      for(size_t chan = 0; chan != boost::gil::size<dst_pixel>::value; chan++)
        a[i][chan] = max - (max - a[i][chan]) * (max - b[i][chan]) / max;
    }
  }

} // namespace images

#endif
