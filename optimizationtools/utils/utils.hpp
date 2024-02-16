#pragma once

#include <algorithm>
#include <random>
#include <sstream>
#include <iterator>

namespace optimizationtools
{

template <typename T>
inline std::vector<T> bob_floyd(
        T sample_size,
        T upper_bound,
        std::mt19937_64& generator)
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

static inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

/**
 * Split a string by whitespace.
 */
inline std::vector<std::string> split(const std::string& line)
{
    std::istringstream buffer(line);
    return {
        std::istream_iterator<std::string>(buffer),
        std::istream_iterator<std::string>()};
}

/**
 * Split a string according to a given separator.
 *
 * Whitespaces are trimmed.
 */
inline std::vector<std::string> split(
        const std::string& line,
        char c)
{
    std::vector<std::string> v;
    std::string line_without_extra_whitespaces;
    unique_copy(line.begin(), line.end(),
            std::back_insert_iterator<std::string>(line_without_extra_whitespaces),
            [](char a, char b){ return isspace(a) && isspace(b);});
    rtrim(line_without_extra_whitespaces);
    ltrim(line_without_extra_whitespaces);
    std::stringstream ss(line_without_extra_whitespaces);
    std::string tmp;
    while (getline(ss, tmp, c)) {
        rtrim(tmp);
        ltrim(tmp);
        v.push_back(tmp);
    }
    return v;
}

inline void hash_combine(
        std::size_t& seed,
        const size_t v)
{
    seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline bool on_segment(
        T x1,
        T y1,
        T x2,
        T y2,
        T x3,
        T y3)
{
    if (x1 <= (std::max)(x1, x3)
            && x2 >= (std::min)(x1, x3)
            && y2 <= (std::max)(y1, y3)
            && y2 >= (std::min)(y1, y3))
        return true;
    return false;
}

template <typename T>
inline int orientation(
        T x1,
        T y1,
        T x2,
        T y2,
        T x3,
        T y3)
{
    auto val = (y2 - y1) * (x3 - x2) - (x2 - x1) * (y3 - y2);
    if (val == 0)
        return 0;
    return (val > 0)? 1: 2;
}

template <typename T>
inline bool intersect(
        T x11,
        T y11,
        T x12,
        T y12,
        T x21,
        T y21,
        T x22,
        T y22)
{
    int o1 = orientation(x11, y11, x12, y12, x21, y21);
    int o2 = orientation(x11, y11, x12, y12, x22, y22);
    int o3 = orientation(x21, y21, x22, y22, x11, y11);
    int o4 = orientation(x21, y21, x22, y22, x12, y12);

    if (o1 != o2 && o3 != o4) {
        if ((x11 == x21 && y11 == y21)
                || (x11 == x22 && y11 == y22)
                || (x12 == x21 && y12 == y21)
                || (x12 == x22 && y12 && y22))
            return false;
        return true;
    }

    if (o1 == 0 && on_segment(x11, y11, x21, y21, x12, y12))
        return true;
    if (o2 == 0 && on_segment(x11, y11, x22, y22, x12, y12))
        return true;
    if (o3 == 0 && on_segment(x21, y21, x11, y11, x22, y22))
        return true;
    if (o4 == 0 && on_segment(x21, y21, x12, y12, x22, y22))
        return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Ratio
{
    Ratio(T v1, T v2): v1(v1), v2(v2) { }

    T v1;
    T v2;
};

/** Output "'v1' / 'v2' ('v1/v2*100'%)". */
template <typename T>
inline std::ostream& operator<<(
        std::ostream& os,
        const Ratio<T>& ratio)
{
    os << ratio.v1 << " / " << ratio.v2 << " (" << (double)ratio.v1 / ratio.v2 * 100 << "%)";
    return os;
}

enum class ObjectiveDirection { Minimize, Maximize };

template <typename T>
inline std::string solution_value(
        ObjectiveDirection objective_direction,
        bool solution_feasible,
        T solution_value)
{
    if (!solution_feasible)
        return (objective_direction == ObjectiveDirection::Minimize)? "inf": "-inf";
    std::stringstream ss;
    ss << solution_value;
    return ss.str();
}

template <typename T>
inline double absolute_optimality_gap(
        ObjectiveDirection objective_direction,
        bool solution_feasible,
        T solution_value,
        T bound)
{
    if (!solution_feasible)
        return std::numeric_limits<double>::infinity();
    return (objective_direction == ObjectiveDirection::Minimize)?
        (double)(solution_value - bound):
        (double)(bound - solution_value);
}

template <typename T>
inline double relative_optimality_gap(
        ObjectiveDirection objective_direction,
        bool solution_feasible,
        T solution_value,
        T bound)
{
    if (!solution_feasible)
        return std::numeric_limits<double>::infinity();
    if (bound == 0)
        return std::numeric_limits<double>::infinity();
    return (objective_direction == ObjectiveDirection::Minimize)?
        (double)(solution_value - bound) / bound:
        (double)(bound - solution_value) / bound;
}

template <typename T>
inline bool is_solution_strictly_better(
        ObjectiveDirection objective_direction,
        bool solution_cur_feasible,
        T solution_cur_value,
        bool solution_new_feasible,
        T solution_new_value)
{
    if (!solution_new_feasible)
        return false;
    if (!solution_cur_feasible)
        return true;
    switch (objective_direction) {
    case ObjectiveDirection::Minimize:
        return solution_new_value < solution_cur_value;
    case ObjectiveDirection::Maximize:
        return solution_new_value > solution_cur_value;
    }
    return false;
}

template <typename T>
inline bool is_solution_strictly_better(
        ObjectiveDirection objective_direction,
        T solution_cur_value,
        bool solution_new_feasible,
        T solution_new_value)
{
    if (!solution_new_feasible)
        return false;
    switch (objective_direction) {
    case ObjectiveDirection::Minimize:
        return solution_new_value < solution_cur_value;
    case ObjectiveDirection::Maximize:
        return solution_new_value > solution_cur_value;
    }
    return false;
}

template <typename T>
inline bool is_solution_better_or_equal(
        ObjectiveDirection objective_direction,
        T solution_cur_value,
        bool solution_new_feasible,
        T solution_new_value)
{
    if (!solution_new_feasible)
        return false;
    switch (objective_direction) {
    case ObjectiveDirection::Minimize:
        return solution_new_value <= solution_cur_value;
    case ObjectiveDirection::Maximize:
        return solution_new_value >= solution_cur_value;
    }
    return false;
}

template <typename T>
inline bool is_value_strictly_better(
        ObjectiveDirection objective_direction,
        T solution_cur_value,
        T solution_new_value)
{
    switch (objective_direction) {
    case ObjectiveDirection::Minimize:
        return solution_new_value < solution_cur_value;
    case ObjectiveDirection::Maximize:
        return solution_new_value > solution_cur_value;
    }
    return false;
}

template <typename T>
inline bool is_bound_strictly_better(
        ObjectiveDirection objective_direction,
        T bound_cur,
        T bound_new)
{
    switch (objective_direction) {
    case ObjectiveDirection::Minimize:
        return bound_new > bound_cur;
    case ObjectiveDirection::Maximize:
        return bound_new < bound_cur;
    }
    return false;
}

}
