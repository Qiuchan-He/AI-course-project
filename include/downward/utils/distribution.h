#ifndef DOWNWARD_UTILS_DISTRIBUTION_H
#define DOWNWARD_UTILS_DISTRIBUTION_H

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/options/option_parser.h"
#include "downward/options/options.h"

#include <memory>
#include <ostream>
#include <random>

namespace utils {
template <typename T>
class Distribution {
protected:
    std::shared_ptr<RandomNumberGenerator> rng;

public:
    Distribution(std::shared_ptr<RandomNumberGenerator> rng);
    Distribution(const options::Options& opts);
    virtual ~Distribution() = default;

    virtual T next() = 0;
    virtual void dump_parameters(std::ostream& stream) = 0;
    virtual void upgrade_parameters();
};

class DiscreteDistribution : public Distribution<int> {
public:
    DiscreteDistribution(std::shared_ptr<RandomNumberGenerator> rng)
        : Distribution(rng)
    {
    }
    DiscreteDistribution(const options::Options& opts)
        : Distribution(opts)
    {
    }
    virtual ~DiscreteDistribution() {}
};

class UniformIntDistribution : public DiscreteDistribution {
    int min;
    int max;
    const float upgrade_min;
    const float upgrade_max;
    std::uniform_int_distribution<int> dist;

public:
    UniformIntDistribution(
        int min,
        int max,
        float upgrade_min,
        float upgrade_max,
        std::shared_ptr<RandomNumberGenerator> rng);
    UniformIntDistribution(const options::Options& opts);
    virtual ~UniformIntDistribution() override;

    virtual int next() override;

    virtual void dump_parameters(std::ostream& stream) override;
    virtual void upgrade_parameters() override;
};
} // namespace utils

#endif /* DISTRIBUTION_H */
