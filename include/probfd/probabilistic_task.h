#ifndef PROBFD_PROBABILISTIC_TASK_H
#define PROBFD_PROBABILISTIC_TASK_H

#include "downward/planning_task.h"

namespace probfd {

/**
 * @brief Interface representing a probabilistic planning task.
 *
 * This class extends PlanningTask by also associating each operator with
 * multiple stochastic outcomes, each having an effect and probability, and a
 * real-valued cost.
 *
 * @warning This class should not be used directly to access individual
 * sub-components of a probabilistic planning task. To this end, use
 * ProbabilisticTaskProxy.
 *
 * @see ProbabilisticTaskProxy
 *
 * @ingroup prob_planning_tasks
 */
class ProbabilisticTask : public PlanningTask {
public:
    virtual ~ProbabilisticTask() override = default;

    /**
     * @brief Get the cost \f$c(a)\f$ of an operator \f$a\f$ with index
     * in \f$\{ 0, ..., |O|-1\}\f$.
     *
     * @remark In contrast to classical planning tasks, probabilistic planning
     * tasks can have real-valued costs.
     */
    virtual double get_operator_cost(int op_index) const = 0;

    /**
     * @brief Get the number of possible outcomes for the probabilistic operator
     * with index in \f$\{ 0, ..., |O|-1\}\f$.
     */
    virtual int get_num_operator_outcomes(int op_index) const = 0;

    /**
     * @brief Get the probability of the outcome with given index of the
     * probabilistic operator with index in \f$\{ 0, ..., |O|-1\}\f$.
     */
    virtual double
    get_operator_outcome_probability(int op_index, int outcome_index) const = 0;

    /**
     * @brief Get the number of effect facts of the outcome with given index of
     * the probabilistic operator with index in \f$\{ 0, ..., |O|-1\}\f$.
     */
    virtual int
    get_num_operator_outcome_effect_facts(int op_index, int outcome_index)
        const = 0;

    /**
     * @brief Get the effect fact with given index of the outcome with given
     * index of the probabilistic operator with index in \f$\{ 0, ...,
     * |O|-1\}\f$.
     */
    virtual FactPair get_operator_outcome_effect_fact(
        int op_index,
        int outcome_index,
        int eff_index) const = 0;
};

} // namespace probfd

#endif // PROBFD_PROBABILISTIC_TASK_H
