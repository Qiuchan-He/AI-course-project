#ifndef DOWNWARD_UTILS_EXCEPTIONS_H
#define DOWNWARD_UTILS_EXCEPTIONS_H

namespace utils {
// Base class for custom exception types.
class Exception {
public:
    virtual ~Exception() = default;
    virtual void print() const = 0;
};

struct TimeoutException {};
}

#endif
