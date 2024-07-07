#ifndef DOWNWARD_OPERATOR_COST_H
#define DOWNWARD_OPERATOR_COST_H

class OperatorProxy;
class OperatorProxy;
namespace options {
class OptionParser;
}

enum OperatorCost {NORMAL = 0, ONE = 1, PLUSONE = 2, MAX_OPERATOR_COST};

int get_adjusted_action_cost(
    const OperatorProxy& op,
    OperatorCost cost_type,
    bool is_unit_cost);

int get_adjusted_action_reward(int reward, OperatorCost cost_type);

void add_cost_type_option_to_parser(options::OptionParser &parser);

#endif
