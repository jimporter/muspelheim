#include "colors.hpp"
#include "ifs.hpp"
#include "muspelheim.hpp"

#include <future>
#include <iostream>
#include <optional>

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/program_options.hpp>

// Put this in the boost namespace so that ADL picks them up (via the
// boost::any parameter).
namespace boost {

template<typename T>
void validate(boost::any &v, const std::vector<std::string> &values,
              std::optional<T>*, int) {
  using namespace boost::program_options;
  using optional_t = std::optional<T>;

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

  bool show_help = false;
  size_t steps = 1000000;
  ptrdiff_t size = 666;
  size_t num_jobs = 1;
  double gamma = 1.0;
  std::optional<double> hdr;

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
      muspelheim::function_system, point2<ptrdiff_t>{size, size}, steps
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
