#ifndef DOWNWARD_TASKS_SYNTACTIC_PROJECTION_H
#define DOWNWARD_TASKS_SYNTACTIC_PROJECTION_H

#include "downward/planning_task.h"
#include "downward/state.h"

#include "downward/pdbs/types.h"

#include <string>
#include <vector>

namespace pdbs {

/**
 * @brief Represents the syntactic projection of a planning task.
 *
 * The syntactic projection of a planning task \f$\Pi = (V, A, I, G)\f$ with
 * respect to a subset of variables \f$P \subseteq V\f$ is the planning task
 * \f$\Pi|_P = (P, A|_P, I|_P, G|_P)\f$ where
 * \f$A|_P = \{ a|_P \mid a \in A\}\f$
 * with \f$\mathit{cost}(a|_P) = \mathit{cost}(a)\f$,
 * \f$\mathit{pre}(a|_P) = \mathit{pre}(a)|_P\f$ and
 * \f$\mathit{eff}(a|_P) = \mathit{eff}(a)|_P\f$.
 *
 * The syntactic projection only considers the variable in \f$P\f$.
 * The operators \f$A|_P\f$ are copies of the operators of the original planning
 * task, in which the variables \f$v \notin P\f$ are removed from the
 * preconditions and effects, effectively ignoring them.
 */
class SyntacticProjection : public ClassicalTask {
    struct ExplicitVariable {
        std::string name;
        int domain_size;
    };

    struct ExplicitOperator {
        std::string name;
        int cost;
        std::vector<FactPair> precondition;
        std::vector<FactPair> effect;
    };

    std::vector<std::vector<std::string>> fact_names;

    std::vector<ExplicitVariable> variables;
    std::vector<ExplicitOperator> operators;
    std::vector<int> initial_state_values;
    std::vector<FactPair> goal_facts;

    std::vector<int> multipliers;

public:
    explicit SyntacticProjection(
        const ClassicalTask& parent_task,
        const Pattern& pattern);

    int get_num_variables() const override;

    std::string get_variable_name(int var) const override;

    int get_variable_domain_size(int var) const override;

    std::string get_fact_name(const FactPair& fact) const override;

    int get_num_operators() const override;

    std::string get_operator_name(int op_index) const override;

    int get_operator_cost(int op_index) const override;

    int get_num_operator_precondition_facts(int op_index) const override;

    FactPair
    get_operator_precondition_fact(int op_index, int fact_index) const override;

    int get_num_operator_effect_facts(int op_index) const override;

    FactPair
    get_operator_effect_fact(int op_index, int eff_index) const override;

    int get_num_goal_facts() const override;

    FactPair get_goal_fact(int index) const override;

    std::vector<int> get_initial_state_values() const override;

    // Internal method.
    bool are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
        const override;

    // Internal method.
    bool is_undefined(const FactPair& fact) const override;

    /// Computes a unique index in the range \f$[0, |S| - 1]\f$ for the input
    /// state.
    int compute_index(const State& state) const;

    /// Computes a unique index in the range \f$[0, |S| - 1]\f$ for the input
    /// state, represented as a list of values for each variable of the
    /// syntactic projection.
    int compute_index(const std::vector<int>& state) const;

    /// Computed the unique state associated with an index in the range
    /// \f$[0, |S| - 1]\f$.
    State get_state_for_index(int index) const;

    /// Get the number of states of the syntactic projection.
    size_t get_num_states() const;
};

} // namespace pdbs

#endif // DOWNWARD_TASKS_PROJECTION_TASK_H
