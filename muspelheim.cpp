#include "variations.hpp"
#include "muspelheim.hpp"
#include "colors.hpp"

#include <iostream>

#include <boost/program_options.hpp>

int main(int argc, const char *argv[]) {
  using namespace muspelheim;
  using namespace boost::gil;
  using rgb8 = rgb8_pixel_t;
  namespace opts = boost::program_options;

  color_theme_iterator colors({20, 80, 200}, 10);
  flame_function_set<rgb8> funcs = {
    { swirl<>(),     { 0.5,   0,     0,     0,     0.5,  -0.5  }, *colors++ },
    { spherical<>(), { 0.48, -0.5,   0.1,  -0.52,  0.48,  0    }, *colors++ },
    { linear<>(),    {-0.5,   0,     0.25, -0.5,   0.25,  0    }, *colors++ },
    { spherical<>(), {-0.25,  0.25,  0.25,  0.25, -0.75,  0.25 }, *colors++ },
    { swirl<>(),     { 0,     0.5,   0.5,   0.5,   0,     0    }, *colors++ },
  };

  bool show_help = false;
  size_t steps = 1000000;
  size_t size = 666;

  opts::options_description desc;
  desc.add_options()
    ("help,h", opts::value(&show_help)->zero_tokens(), "show help")
    ("steps,n", opts::value(&steps), "number of iterations")
    ("size,s", opts::value(&size), "image size")
  ;

  try {
    auto parsed = opts::command_line_parser(argc, argv).options(desc).run();

    opts::variables_map vm;
    opts::store(parsed, vm);
    opts::notify(vm);
  } catch(const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 2;
  }

  if(show_help) {
    std::cout << desc << std::endl;
    return 0;
  }

  rgb8_image_t image(size, size, rgb8(0), 0);
  chaos_game(view(image), funcs, steps);
  png_write_view("output.png", const_view(image));

  return 0;
}
