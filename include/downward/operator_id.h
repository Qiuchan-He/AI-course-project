#ifndef DOWNWARD_OPERATOR_ID_H
#define DOWNWARD_OPERATOR_ID_H

#include "downward/utils/hash.h"

#include <iostream>

/*
  OperatorIDs are used to define an operator that belongs to a given
  planning task. These IDs are meant to be compact and efficient to use.
  They can be thought of as a type-safe replacement for "int" for the
  purpose of referring to an operator.

  Because of their efficiency requirements, they do *not* store which
  task they belong to, and it is the user's responsibility not to mix
  OperatorIDs that belong to different tasks.
*/
class OperatorID {
    int index;

public:
    explicit OperatorID(int index)
        : index(index)
    {
    }

    static const OperatorID no_operator;

    int get_index() const { return index; }

    bool operator==(const OperatorID& other) const
    {
        return index == other.index;
    }

    bool operator!=(const OperatorID& other) const { return !(*this == other); }

    int hash() const { return index; }
};

std::ostream& operator<<(std::ostream& os, OperatorID id);

namespace utils {
inline void feed(HashState& hash_state, OperatorID id)
{
    feed(hash_state, id.hash());
}
} // namespace utils

#endif
