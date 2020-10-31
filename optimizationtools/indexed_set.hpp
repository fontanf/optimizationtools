#pragma once

#include <vector>
#include <cstdint>
#include <cassert>

namespace optimizationtools
{

/*

Removing elements while iterating:

for (auto it = set.begin(); it != set.end();) {
    bool remove = true;
    // ...
    if (remove) {
        solution.remove(*it);
    } else {
        it++;
    }
}

*/

class IndexedSet
{

public:

    typedef int64_t Index;
    typedef int64_t Position;

    inline IndexedSet(Index element_number);
    inline virtual ~IndexedSet() { }

    inline bool empty() const { return element_number_ == 0; }
    inline Position size() const { return element_number_; }
    inline bool contains(Index index) const { return (positions_[index] < element_number_); }
    inline Position position(Index index) const { return positions_[index]; }

    inline std::vector<Index>::const_iterator begin() const { return elements_.begin(); }
    inline std::vector<Index>::const_iterator end() const { return elements_.begin() + element_number_; }
    inline std::vector<Index>::const_iterator out_begin() const { return elements_.begin() + element_number_; }
    inline std::vector<Index>::const_iterator out_end() const { return elements_.end(); }

    inline bool add(Index index);
    inline bool remove(Index index);
    inline void clear() { element_number_ = 0; }

    inline bool check() const;

private:

    std::vector<Index> elements_;
    std::vector<Position> positions_;
    Position element_number_ = 0;

};

/******************************************************************************/

inline IndexedSet::IndexedSet(Index element_number):
    elements_(element_number),
    positions_(element_number)
{
    for (Index index = 0; index < element_number; ++index) {
        elements_[index] = index;
        positions_[index] = index;
    }
}

inline bool IndexedSet::add(Index index)
{
    Position position = positions_[index];
    if (position < element_number_)
        return false;
    elements_[position] = elements_[element_number_];
    elements_[element_number_] = index;
    positions_[elements_[position]] = position;
    positions_[elements_[element_number_]] = element_number_;
    element_number_++;
    return true;
}

inline bool IndexedSet::remove(Index index)
{
    Position position = positions_[index];
    if (position >= element_number_)
        return false;
    elements_[position] = elements_[element_number_ - 1];
    elements_[element_number_ - 1] = index;
    positions_[elements_[position]] = position;
    positions_[elements_[element_number_ - 1]] = element_number_ - 1;
    element_number_--;
    return true;
}

}

