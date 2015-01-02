#include "variations.hpp"
#include "muspelheim.hpp"
#include "colors.hpp"

#include <future>
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
  size_t num_jobs = 1;
  double gamma = 1.0;

  opts::options_description desc;
  desc.add_options()
    ("help,h", opts::value(&show_help)->zero_tokens(), "show help")
    ("steps,n", opts::value(&steps), "number of iterations")
    ("size,s", opts::value(&size), "image size")
    ("jobs,j", opts::value(&num_jobs), "number of parallel jobs")
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

  std::vector< std::future<raw_image_data<rgb8>> > jobs;
  for (size_t i = 0; i < num_jobs; i++) {
    jobs.push_back(std::async(
      std::launch::async, chaos_game<rgb8>,
      funcs, point2<ptrdiff_t>{size, size}, steps
    ));
  }
  std::vector<raw_image_data<rgb8>> data;
  for (auto &job : jobs)
    data.push_back(job.get());
  auto combined = combine(data);

  rgb8_image_t image(size, size, rgb8(0), 0);
  render(view(image), log_alpha(combined), gamma);
  png_write_view("output.png", const_view(image));

  return 0;
}
