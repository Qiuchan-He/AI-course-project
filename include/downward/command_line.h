#ifndef DOWNWARD_COMMAND_LINE_H
#define DOWNWARD_COMMAND_LINE_H

#include "downward/utils/exceptions.h"

#include <memory>
#include <string>

namespace options {
class Registry;
}

class SearchAlgorithm;

class ArgError : public utils::Exception {
    std::string msg;

public:
    explicit ArgError(const std::string& msg);

    virtual void print() const override;
};

extern std::shared_ptr<SearchAlgorithm> parse_cmd_line(
    int argc,
    const char** argv,
    options::Registry& registry,
    bool dry_run,
    bool is_unit_cost);

extern std::string usage(const std::string& progname);

#endif
