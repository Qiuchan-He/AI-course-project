#include "q_learning/experience_sample.h"

#include <stdexcept>

using namespace std;
using namespace probfd;

namespace q_learning {

namespace {
template <typename T, typename A>
std::istream& operator>>(std::istream& in, std::vector<T, A>& list)
{
    list.clear();
    if (in.get() != '[') {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> std::ws;

    if (in.peek() == ']') {
        in.get();
        return in;
    }

    if (!(in >> list.emplace_back())) return in;
    in >> std::ws;

    for (int c = in.get(); c != ']'; c = in.get()) {
        if (c != ',') {
            in.setstate(std::ios::failbit);
            return in;
        }
        if (!(in >> list.emplace_back())) return in;
        in >> std::ws;
    }

    return in;
}
} // namespace

static ProbabilisticOperatorProxy get_operator_by_name(
    ProbabilisticTaskProxy task_proxy,
    std::string_view action_name)
{
    ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    for (auto op : operators) {
        if (op.get_name() == action_name) {
            return op;
        }
    }

    std::cerr << "Could not find an operator with the name: " << action_name
              << std::endl;
    utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
}

ExperienceSample::ExperienceSample(ProbabilisticTask& task, std::istream& in)
    : state(
          task,
          [&] {
              std::vector<int> state_values;
              if (!(in >> std::ws >> state_values))
                  throw std::invalid_argument("");
              return state_values;
          }())
    , action([&] {
        std::string action_name;
        in >> std::ws;
        if (in.get() != '(') throw std::invalid_argument("");
        if (!std::getline(in, action_name, ')'))
            throw std::invalid_argument("");
        ProbabilisticTaskProxy proxy(task);
        return get_operator_by_name(proxy, action_name).get_id();
    }())
    , reward([&] {
        double reward;
        if (!(in >> std::ws >> reward)) throw std::invalid_argument("");
        return reward;
    }())
    , successor(
          task,
          [&] {
              std::vector<int> state_values;
              if (!(in >> std::ws >> state_values))
                  throw std::invalid_argument("");
              return state_values;
          }())
    , terminal([&] {
        bool terminal;
        if (!(in >> std::ws >> std::boolalpha >> terminal))
            throw std::invalid_argument("");
        return terminal;
    }())
{
}

ExperienceSample::ExperienceSample(
    State state,
    OperatorID action,
    double reward,
    State successor,
    bool terminal)
    : state(std::move(state))
    , action(action)
    , reward(reward)
    , successor(std::move(successor))
    , terminal(terminal)
{
}

void ExperienceSample::dump(PlanningTaskProxy task_proxy, std::ostream& out)
{
    out << state.get_values() << "\n"
        << "(" << task_proxy.get_abstract_operators()[action].get_name()
        << ")\n"
        << reward << "\n"
        << successor.get_values() << "\n"
        << std::boolalpha << terminal << std::endl;
}

} // namespace q_learning
