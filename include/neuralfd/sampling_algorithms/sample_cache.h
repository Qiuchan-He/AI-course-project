#ifndef NEURALFD_SEARCH_ALGORITHMS_SAMPLE_CACHE_H
#define NEURALFD_SEARCH_ALGORITHMS_SAMPLE_CACHE_H

#include "downward/utils/hash.h"

#include <string>
#include <vector>

namespace sampling_algorithm {
class SamplingAlgorithm;

class SampleCache {
public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const std::string;
        using pointer = const std::string*;
        using reference = const std::string&;

        Iterator(
            bool unique,
            std::vector<std::string>::iterator iter_vector,
            std::unordered_set<std::string>::iterator iter_set);
        reference operator*() const;
        pointer operator->();
        Iterator& operator++();
        Iterator operator++(int);

        friend bool operator==(const Iterator& lhs, const Iterator& rhs);

        std::vector<std::string>::iterator get_iter_vector();
        std::unordered_set<std::string>::iterator get_iter_set();

    private:
        const bool unique;
        std::vector<std::string>::iterator iter_vector;
        std::unordered_set<std::string>::iterator iter_set;
    };

private:
    const bool unique_samples;
    std::vector<std::string> redundant_cache;
    std::unordered_set<std::string> unique_cache;

public:
    SampleCache(bool unique_samples);

    template <class AnyIterator>
    void insert(AnyIterator first, AnyIterator last)
    {
        if (unique_samples) {
            unique_cache.insert(first, last);
        } else {
            redundant_cache.insert(redundant_cache.end(), first, last);
        }
    }
    void erase(Iterator first, Iterator last);
    Iterator begin();
    Iterator end();
    std::size_t size() const;
};

class SampleCacheManager {
protected:
    SampleCache sample_cache;
    const size_t max_size;

    const bool iterate_sample_files;
    const int max_sample_files;

    int index_sample_files;
    sampling_algorithm::SamplingAlgorithm* algorithm;
    int nb_files_written = 0;
    int nb_samples_written = 0;
    bool is_finalized = false;

    void write_to_disk();

public:
    SampleCacheManager(
        SampleCache sample_cache,
        int max_size,
        bool iterate_sample_files,
        int max_sample_files,
        int index_sample_files,
        SamplingAlgorithm* algorithm);
    template <class AnyIterator>
    void insert(AnyIterator first, AnyIterator last)
    {
        assert(!is_finalized);
        sample_cache.insert(first, last);
        if (sample_cache.size() >= max_size) {
            write_to_disk();
        }
    }
    void finalize();
    size_t size() const;
};
} // namespace sampling_algorithm
#endif
