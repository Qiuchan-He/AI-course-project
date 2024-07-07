#ifndef DOWNWARD_HEURISTIC_H
#define DOWNWARD_HEURISTIC_H

#include "downward/evaluator.h"
#include "downward/operator_id.h"
#include "downward/per_state_information.h"
#include "downward/task_proxy.h"

#include "downward/algorithms/ordered_set.h"

#include <memory>
#include <utility>
#include <vector>

class ClassicalTaskProxy;

namespace options {
class OptionParser;
class Options;
} // namespace options

/**
 * @brief Represents a classical planning heuristic for a given classical
 * planning task.
 *
 * @ingroup heuristics
 */
class Heuristic : public Evaluator {
protected:
    struct HEntry {
        /* dirty is conceptually a bool, but Visual C++ does not support
           packing ints and bools together in a bitfield. */
        int h : 31;
        unsigned int dirty : 1;

        HEntry(int h, bool dirty)
            : h(h)
            , dirty(dirty)
        {
        }
    };
    static_assert(sizeof(HEntry) == 4, "HEntry has unexpected size.");

    /*
      TODO: We might want to get rid of the preferred_operators
      attribute. It is currently only used by compute_result() and the
      methods it calls (compute_heuristic() directly, further methods
      indirectly), and we could e.g. change this by having
      compute_heuristic return an EvaluationResult object.

      If we do this, we should be mindful of the cost incurred by not
      being able to reuse the data structure from one iteration to the
      next, but this seems to be the only potential downside.
    */
    ordered_set::OrderedSet<OperatorID> preferred_operators;

    /*
      Cache for saving h values - as soon as the cache is
      accessed it will create entries for all existing states
    */
    PerStateInformation<HEntry> heuristic_cache;

    /// The planning task for which this heuristic is computed.
    const std::shared_ptr<ClassicalTask> task;

    /// A task proxy object to traverse individual sub-components of the
    /// planning task.
    ClassicalTaskProxy task_proxy;

public:
    /// Heuristic value representing positive infinity (dead end).
    static constexpr int DEAD_END = -1;
    static constexpr int NO_VALUE = -2;
    
    explicit Heuristic(const options::Options& opts);

    /**
     * @brief Constructs the heuristic.
     * 
     * @param description - A description string for the heuristic for logging
     * purposes
     * @param log - A log proxy for logging purposes
     * @param task - The classical planning task for which the heuristic is
     * computed
     */
    explicit Heuristic(
        std::string description,
        utils::LogProxy log,
        std::shared_ptr<ClassicalTask> task);

    virtual ~Heuristic() override;

    /**
     * @brief Evaluates the heuristic on the given input state and returns the
     * computed heuristic value. 
     * 
     * @remark The input state is assumed to be a state for the planning task 
     * stored internally by the heuristic.
     */
    virtual int compute_heuristic(const State& ancestor_state) = 0;

    // Needed by NeuralFD
    virtual std::pair<int, double>
    compute_heuristic_and_confidence(const State& state);

    virtual void
    get_path_dependent_evaluators(std::set<Evaluator*>& evals) override;

    virtual EvaluationResult
    compute_result(EvaluationContext& eval_context) override;

    virtual bool is_estimate_cached(const State& state) const override;
    virtual int get_cached_estimate(const State& state) const override;

    static void add_options_to_parser(options::OptionParser& parser);
};

#endif // DOWNWARD_HEURISTIC_H
