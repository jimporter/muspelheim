#ifndef INC_MUSPELHEIM_MUSPELHEIM_HPP
#define INC_MUSPELHEIM_MUSPELHEIM_HPP

#include "colors.hpp"
#include "ifs.hpp"
#include "variations.hpp"

#include <boost/gil/typedefs.hpp>

namespace muspelheim {

using rgb8 = boost::gil::rgb8_pixel_t;
using flame_function_system = ifs::iterated_function_system<rgb8>;

extern flame_function_system function_system;

} // namespace muspelheim

#endif
