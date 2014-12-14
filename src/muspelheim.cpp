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

  color_theme_iterator colors({40, 140, 140}, 6);
  flame_function_set<rgb8> funcs = {
    { handkerchief, translate(0.5, -0.75)*identity(), *colors++ },
    { handkerchief, translate(0.75, -0.5)*rotate(M_PI), *colors++ },
    { linear, translate(-0.1, 0.1)*scale(0.3), *colors++ },
    { linear, translate(0.1, -0.1)*scale(0.35)*rotate(M_PI/2), *colors++ },
    { spiral, translate(-0.5, 0.1)*scale(0.5), *colors++ },
    { spiral, translate(0.5, -0.1)*scale(0.5), *colors++ },
    { spiral, translate(-0.5, 0.1)*scale(0.5)*rotate(M_PI/2), *colors++ },
    { spiral, translate(0.5, -0.1)*scale(0.5)*rotate(M_PI/2), *colors++ },
    { spiral, translate(0.1, -0.5)*scale(0.5), *colors++ },
    { spiral, translate(-0.1, 0.5)*scale(0.5), *colors++ },
    { spiral, translate(0.1, -0.5)*scale(0.5)*rotate(M_PI/2), *colors++ },
    { spiral, translate(-0.1, 0.5)*scale(0.5)*rotate(M_PI/2), *colors++ },
    { swirl, translate(-0.5, -0.5)*scale(-0.75), *colors++ },
  };

  bool show_help = false;
  size_t steps = 1000000;
  ptrdiff_t size = 666;
  double gamma = 1.0;

  opts::options_description desc;
  desc.add_options()
    ("help,h", opts::value(&show_help)->zero_tokens(), "show help")
    ("steps,n", opts::value(&steps), "number of iterations")
    ("size,s", opts::value(&size), "image size")
    ("gamma,g", opts::value(&gamma), "gamma adjustment")
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
  render(
    view(image),
    log_alpha(chaos_game(funcs, {size, size}, steps)),
    gamma
  );
  png_write_view("output.png", const_view(image));

  return 0;
}
