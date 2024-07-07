#ifndef DOWNWARD_EVALUATOR_H
#define DOWNWARD_EVALUATOR_H

#include "downward/evaluation_result.h"

#include "downward/utils/logging.h"

#include <set>
#include <vector>

class EvaluationContext;
class State;

namespace options {
class OptionParser;
class Options;
} // namespace options

class Evaluator {
    const std::string description;

protected:
    mutable utils::LogProxy log;

public:
    explicit Evaluator(const options::Options& opts);
    explicit Evaluator(std::string description, utils::LogProxy log);

    virtual ~Evaluator() = default;

    /*
      dead_ends_are_reliable should return true if the evaluator is
      "safe", i.e., infinite estimates can be trusted.

      The default implementation returns true.
    */
    virtual bool dead_ends_are_reliable() const;

    /*
      get_path_dependent_evaluators should insert all path-dependent
      evaluators that this evaluator directly or indirectly depends on
      into the result set, including itself if necessary.

      The two notify methods below will be called for these and only
      these evaluators. In other words, "path-dependent" for our
      purposes means "needs to be notified of the initial state and
      state transitions".
    */
    virtual void get_path_dependent_evaluators(std::set<Evaluator*>& evals) = 0;

    virtual void notify_initial_state(const State& /*initial_state*/) {}

    virtual void notify_state_transition(
        const State& /*parent_state*/,
        OperatorID /*op_id*/,
        const State& /*state*/)
    {
    }

    /*
      compute_result should compute the estimate and possibly
      preferred operators for the given evaluation context and return
      them as an EvaluationResult instance.

      It should not add the result to the evaluation context -- this
      is done automatically elsewhere.

      The passed-in evaluation context is not const because this
      evaluator might depend on other evaluators, in which case their
      results will be stored in the evaluation context as a side
      effect (to make sure they are only computed once).

      TODO: We should make sure that evaluators don't directly call
      compute_result for their subevaluators, as this could circumvent
      the caching mechanism provided by the EvaluationContext. The
      compute_result method should only be called by
      EvaluationContext. We need to think of a clean way to achieve
      this.
    */
    virtual EvaluationResult
    compute_result(EvaluationContext& eval_context) = 0;

    // Needed by NeuralFD
    virtual std::vector<EvaluationResult>
    compute_results(std::vector<EvaluationContext>& eval_contexts);

    void report_value_for_initial_state(const EvaluationResult& result) const;
    void report_new_minimum_value(const EvaluationResult& result) const;

    const std::string& get_description() const;

    virtual bool does_cache_estimates() const;
    virtual bool is_estimate_cached(const State& state) const;
    /*
      Calling get_cached_estimate is only allowed if an estimate for
      the given state is cached, i.e., is_estimate_cached returns true.
    */
    virtual int get_cached_estimate(const State& state) const;
};

extern void add_evaluator_options_to_parser(options::OptionParser& parser);

#endif
