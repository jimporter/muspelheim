#include "../muspelheim.hpp"

using namespace math;
using namespace muspelheim;

static rgb8 white(255, 255, 255);
static double sqrt3 = std::sqrt(3.0);
flame_function_system muspelheim::function_system = {
  { linear, rotate(M_PI/6)              * scale(1/sqrt3), white },
  { linear, translate( 1/sqrt3,  1/3.0) * scale(1/3.0),   white },
  { linear, translate(       0,  2/3.0) * scale(1/3.0),   white },
  { linear, translate(-1/sqrt3,  1/3.0) * scale(1/3.0),   white },
  { linear, translate(-1/sqrt3, -1/3.0) * scale(1/3.0),   white },
  { linear, translate(       0, -2/3.0) * scale(1/3.0),   white },
  { linear, translate( 1/sqrt3, -1/3.0) * scale(1/3.0),   white },
};
