#pragma once

#include <algorithm>
#include <random>
#include <sstream>

namespace optimizationtools
{

template <typename T>
std::vector<T> bob_floyd(T sample_size, T upper_bound, std::mt19937_64& generator);

inline std::vector<std::string> split(std::string line, char c = ',');

/******************************************************************************/

template <typename T>
std::vector<T> bob_floyd(T sample_size, T upper_bound, std::mt19937_64& generator)
{
    std::vector<T> samples;
    for (T d = upper_bound - sample_size; d < upper_bound; d++) {
        T t = std::uniform_int_distribution<T>(0, d)(generator);
        if (std::find(samples.begin(), samples.end(), t) == samples.end()) {
            samples.push_back(t);
        } else {
            samples.push_back(d);
        }
    }
    return samples;
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::vector<std::string> split(std::string line, char c)
{
    std::vector<std::string> v;
    std::stringstream ss(line);
    std::string tmp;
    while (getline(ss, tmp, c)) {
        rtrim(tmp);
        v.push_back(tmp);
    }
    return v;
}

}

