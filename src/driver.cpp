#include "colors.hpp"
#include "ifs.hpp"
#include "variations.hpp"

#include <future>
#include <iostream>
#include <experimental/optional>

#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/program_options.hpp>

// Put this in the boost namespace so that ADL picks them up (via the
// boost::any parameter).
namespace boost {

template<typename T>
void validate(boost::any &v, const std::vector<std::string> &values,
              std::experimental::optional<T>*, int) {
  using namespace boost::program_options;
  using optional_t = std::experimental::optional<T>;

  if(v.empty())
    v = optional_t();
  auto *val = boost::any_cast<optional_t>(&v);
  assert(val);

  boost::any a;
  validate(a, values, static_cast<T*>(nullptr), 0);
  *val = boost::any_cast<T>(a);
}

} // namespace boost

int main(int argc, const char *argv[]) {
  using namespace math;
  using namespace boost::gil;
  using rgb8 = rgb8_pixel_t;
  namespace opts = boost::program_options;

  colors::color_theme_iterator colors({40, 140, 140}, 6);
  ifs::iterated_function_system<rgb8> funcs = {
    { handkerchief, translate(0.5, -0.75)*identity(), *colors++ },
    { handkerchief, translate(0.75, -0.5)*rotate(M_PI), *colors++ },
    { linear, translate(-0.1, 0.1)*scale(0.3), *colors++ },
    { linear, translate(0.1, -0.1)*scale(0.35)*rotate(M_PI/2), *colors++ },
    { spiral, translate(-0.5, 0.1)*scale(0.5), *colors++,
      translate(0.1, 0.9) },
    { spiral, translate(0.5, -0.1)*scale(0.5), *colors++,
      translate(0.2, -0.8) },
    { spiral, translate(-0.5, 0.1)*scale(0.5)*rotate(M_PI/2), *colors++,
      translate(-0.3, -0.7) },
    { spiral, translate(0.5, -0.1)*scale(0.5)*rotate(M_PI/2), *colors++,
      translate(-0.4, 0.6) },
    { spiral, translate(0.1, -0.5)*scale(0.5), *colors++,
      translate(0.5, 0.5) },
    { spiral, translate(-0.1, 0.5)*scale(0.5), *colors++,
      translate(0.6, -0.4) },
    { spiral, translate(0.1, -0.5)*scale(0.5)*rotate(M_PI/2), *colors++,
      translate(-0.7, -0.3) },
    { spiral, translate(-0.1, 0.5)*scale(0.5)*rotate(M_PI/2), *colors++,
      translate(-0.8, 0.2) },
    { swirl, translate(-0.5, -0.5)*scale(-0.75), *colors++ },
  };

  bool show_help = false;
  size_t steps = 1000000;
  ptrdiff_t size = 666;
  size_t num_jobs = 1;
  double gamma = 1.0;
  std::experimental::optional<double> hdr;

  opts::options_description desc;
  desc.add_options()
    ("help,h", opts::value(&show_help)->zero_tokens(), "show help")
    ("steps,n", opts::value(&steps), "number of iterations")
    ("size,s", opts::value(&size), "image size")
    ("jobs,j", opts::value(&num_jobs), "number of parallel jobs")
    ("gamma,g", opts::value(&gamma), "gamma adjustment")
    ("hdr,H", opts::value(&hdr)->implicit_value(1.0, "1.0"), "enable HDR")
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

  std::vector< std::future<images::raw_image_data<rgb8>> > jobs;
  for(size_t i = 0; i < num_jobs; i++) {
    jobs.push_back(std::async(
      std::launch::async, ifs::chaos_game<rgb8>,
      funcs, point2<ptrdiff_t>{size, size}, steps
    ));
  }
  std::vector<images::raw_image_data<rgb8>> data;
  for(auto &job : jobs)
    data.push_back(job.get());
  auto combined = images::combine(data);

  rgb8_image_t image(size, size, rgb8(0), 0);
  images::render(view(image), images::log_alpha(combined), gamma);

  if(hdr) {
    rgb8_image_t gray(size, size, rgb8(0), 0);
    images::render_monochrome(
      view(gray), images::linear_alpha(combined), rgb8(255, 255, 255), *hdr
    );
    images::lighten(view(image), const_view(gray));
  }

  png_write_view("output.png", const_view(image));

  return 0;
}
