#ifndef Q_LEARNING_FP_COMPARE_H
#define Q_LEARNING_FP_COMPARE_H

#include <limits>
#include <string>

namespace probfd {

/**
 * \addtogroup probfd
 *  @{
 */

static constexpr double EPSILON = 1e-5;

/// Equivalent to \f$|v_1 - v_2| \leq \epsilon\f$
bool is_approx_equal(double v1, double v2, double epsilon = EPSILON);

/** @} */

} // namespace probfd

#endif // __value_type_H__
