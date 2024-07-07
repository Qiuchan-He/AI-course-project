#include "q_learning/fp_compare.h"

#include <cassert>
#include <cmath>

namespace probfd {

bool is_approx_equal(double v1, double v2, double tolerance)
{
    assert(tolerance >= 0.0);
    return v1 == v2 || std::abs(v1 - v2) <= tolerance;
}

} // namespace probfd
