#ifndef INC_MUSPELHEIM_IFS_HPP
#define INC_MUSPELHEIM_IFS_HPP

#include <functional>
#include <random>
#include <vector>

#include "images.hpp"
#include "vec2d.hpp"

namespace ifs {

  template<typename Pixel>
  class iterated_function {
  public:
    using pixel_type = Pixel;
    using function_type = std::function<
      math::vec2d(const math::vec2d &, const math::affine_transform &)
    >;
    using value_type = std::pair<function_type, double>;

    iterated_function(
      const function_type &f, const math::affine_transform &transform,
      const pixel_type &color,
      const math::affine_transform &post = math::identity()
    ) : iterated_function({{f, 1}}, transform, color, post) {}

    iterated_function(
      const std::initializer_list<value_type> &f,
      const math::affine_transform &transform, const pixel_type &color,
      const math::affine_transform &post = math::identity()
    ) : f_(f), transform_(transform), color_(color), post_(post) {}

    inline math::vec2d operator ()(const math::vec2d &p) const {
      math::vec2d value = {0, 0};
      const auto &transformed = transform_(p);
      for(const auto &i : f_)
        value += i.second * i.first(transformed, transform_);
      return post_(value);
    }

    inline const pixel_type & color() const {
      return color_;
    }
  private:
    std::vector<value_type> f_;
    math::affine_transform transform_;
    pixel_type color_;
    math::affine_transform post_;
  };

  template<typename Pixel>
  using iterated_function_system = std::vector<iterated_function<Pixel>>;

  template<typename Pixel>
  images::raw_image_data<Pixel>
  chaos_game(const iterated_function_system<Pixel> &funcs,
             const boost::gil::point2<ptrdiff_t> &dimensions,
             size_t num_iterations = 10000000) {
    using namespace boost::gil;
    using image_pt = point2<ptrdiff_t>;

    std::default_random_engine engine(std::random_device{}());
    std::uniform_int_distribution<size_t> random_func(0, funcs.size() - 1);
    std::uniform_real_distribution<double> random_biunit(-1, 1);

    images::raw_image_data<Pixel> result(dimensions);
    auto color = view(result.color);
    auto alpha = view(result.alpha);

    math::vec2d point(random_biunit(engine), random_biunit(engine));

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
        color(pt) = images::blend(color(pt), f.color(), 0.9);

      alpha(pt)++;
    }

    return result;
  }

} // namespace ifs

#endif
