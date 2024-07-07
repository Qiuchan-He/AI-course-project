#ifndef PROBFD_Q_LEARNING_EXPERIENCE_SAMPLE_H
#define PROBFD_Q_LEARNING_EXPERIENCE_SAMPLE_H

#include "probfd/task_proxy.h"

#include "downward/operator_id.h"

#include "downward/state.h"

#include <istream>
#include <ostream>

namespace q_learning {

/**
 * @brief This struct represents a sample \f$(s, a, r, s')\f$ of the
 * environment.
 *
 * Here, \f$s\f$ is the source state, \f$a\f$ is an action, \$r\$ is a reward
 * and \f$s'\f$ is the successor state of \f$s\f$ after applying \f$a\f$.
 *
 * @ingroup q_learning
 */
struct ExperienceSample {
    State state;       ///< The source state \f$s\f$.
    OperatorID action; ///< The action \f$a\f$ applies in \f$s\f$.
    double reward;     ///< The reward \f$r\f$ that was received.
    State successor;   ///< The resulting successor state \f$s'\f$.
    bool terminal;     ///< Whether the successor state \f$s'\f$ is terminal.

    // Read the experience sample from an input stream.
    ExperienceSample(probfd::ProbabilisticTask& task, std::istream& in);

    /// Constructs an experience sample.
    ExperienceSample(
        State state,
        OperatorID action,
        double reward,
        State successor,
        bool terminal);

    /// Checks if two experience samples are equal component-wise.
    friend bool
    operator==(const ExperienceSample&, const ExperienceSample&) = default;

    void dump(PlanningTaskProxy task_proxy, std::ostream& out);
};

} // namespace q_learning

#endif // PROBFD_Q_LEARNING_EXPERIENCE_SAMPLE_H
