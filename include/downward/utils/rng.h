#ifndef DOWNWARD_UTILS_RNG_H
#define DOWNWARD_UTILS_RNG_H

#include <algorithm>
#include <bit>
#include <cassert>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

namespace utils {

/**
 * @brief Implements a random number generator.
 *
 * @note This implementation is consistent across all platforms with exactly the
 * same behaviour so long as the same random seed is provided.
 */
class RandomNumberGenerator {
    // Mersenne Twister random number generator.
    std::mt19937 rng;

public:
    RandomNumberGenerator(); // Seed with a value depending on time and process
                             // ID.

    /**
     * @brief Constructs a new rng initialized with the given random seed.
     */
    explicit RandomNumberGenerator(int seed);

    RandomNumberGenerator(const RandomNumberGenerator&) = delete;
    RandomNumberGenerator& operator=(const RandomNumberGenerator&) = delete;
    ~RandomNumberGenerator();

    void seed(int seed);

    double random();

    double get_uniform(std::uniform_int_distribution<int>& dist);

    /// Returns random integer in [0..bound).
    int random(int bound);

    double gaussian(double mean = 0.0, double stddev = 1.0)
    {
        std::normal_distribution<double> distribution(mean, stddev);
        return distribution(rng);
    }

    template <typename T>
    typename T::const_iterator choose(const T& vec)
    {
        return vec.begin() + random(vec.size());
    }

    template <typename T>
    typename T::iterator choose(T& vec)
    {
        return vec.begin() + random(vec.size());
    }

    template <typename W>
    size_t weighted_choose_index(const std::vector<W>& weights)
    {
        assert(all_of(weights.begin(), weights.end(), [](W i) {
            return i >= 0.0;
        }));
        double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
        assert(sum > 0.0);
        double choice = random() * sum;
        for (size_t i = 0; i < weights.size(); ++i) {
            choice -= weights[i];
            if (choice < 0) {
                return i;
            }
        }
        assert(false);
        return 0;
    }

    template <typename T, typename W>
    typename std::vector<T>::const_iterator
    weighted_choose(const std::vector<T>& vec, const std::vector<W>& weights)
    {
        assert(vec.size() == weights.size());
        return vec.begin() + weighted_choose_index(weights);
    }

    template<typename T>
    void shuffle(std::vector<T>& vec)
    {
        std::shuffle(vec.begin(), vec.end(), rng);
    }

    std::vector<int> choose_n_of_N(int n, int N);
};
} // namespace utils

#endif
