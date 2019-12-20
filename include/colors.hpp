#ifndef INC_MUSPELHEIM_COLORS_HPP
#define INC_MUSPELHEIM_COLORS_HPP

#include <boost/gil/typedefs.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace colors {

struct rgb {
  rgb() = default;
  rgb(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

  uint8_t r, g, b;

  operator boost::gil::rgb8_pixel_t() const {
    return boost::gil::rgb8_pixel_t(r, g, b);
  }
};

struct hsv {
  hsv() = default;
  hsv(int16_t h, uint8_t s, uint8_t v) : h(h), s(s), v(v) {}

  int16_t h;
  uint8_t s, v;
};

hsv tohsv(const rgb &value);
rgb torgb(const hsv &value);

class color_theme_iterator : public boost::iterator_facade<
  color_theme_iterator, const rgb, boost::forward_traversal_tag
> {
public:
  color_theme_iterator() {}

  color_theme_iterator(const rgb &base, int16_t spread = 10)
    : base_(tohsv(base)), spread_(spread), index_(1) {
    evaluate();
  }
private:
  friend class boost::iterator_core_access;

  void increment() {
    index_++;
    evaluate();
  }

  bool equal(const color_theme_iterator &other) const {
    return false; // TODO
  }

  const rgb & dereference() const {
    return curr_;
  }

  void evaluate();

  hsv base_;
  int16_t spread_;
  int index_;
  rgb curr_;
};

} // namespace colors

#endif
