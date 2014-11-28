#include "variations.hpp"
#include "muspelheim.hpp"

int main() {
  using namespace muspelheim;
  using namespace boost::gil;

  flame_function_set<rgb8_pixel_t> funcs = {
    { spiral<>   ({0.5,  0, 0,     0, 0.5, 0   }), rgb8_pixel_t(255, 83, 13) },
    { linear<>   ({0,    1, 0,     1, 0,   0   }), rgb8_pixel_t(234, 44, 12) },
    { linear<>   ({-1,   0, -0.25,  0, -0.75, 0.25}), rgb8_pixel_t(255, 0, 0) },
    { spherical<>({-0.5, 0, 0,     0, 0.5, 0.5 }), rgb8_pixel_t(232, 12, 122) },
    { spherical<>({0.5,  0, 0.5,   0, 0.5, 0.5 }), rgb8_pixel_t(255, 13, 255) }
  };

  rgb8_image_t image(666, 666, rgb8_pixel_t(0), 0);
  chaos_game(view(image), funcs, 100000000);
  png_write_view("output.png", const_view(image));

  return 0;
}