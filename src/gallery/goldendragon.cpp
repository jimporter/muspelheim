#include "../muspelheim.hpp"

using namespace math;
using namespace muspelheim;

static double r = 0.74274;
static double A = 0.574105;
static double B = 2.321532;

flame_function_system muspelheim::function_system = {
  { linear, scale(1/1.5) * translate(-0.5, 0)*scale(r)*rotate(A) * scale(1.5),
    rgb8(255, 255, 255) },
  { linear, scale(1/1.5) * translate(0.5, 0)*scale(r*r)*rotate(B) * scale(1.5),
    rgb8(255, 255, 255) },
};
