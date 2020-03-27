#pragma once

#include <vector>
#include <cstdint>

namespace optimizationtools
{

class IndexedSet
{

public:

    typedef int64_t Index;
    typedef int64_t Position;

    IndexedSet(Index element_number): positions_(element_number, -1) { }
    virtual ~IndexedSet() { }

    inline bool empty() const { return set_.size() == 0; }
    inline Position size() const { return set_.size(); }
    inline Index operator[](Position position) { return set_[position]; }
    inline bool contains(Index index) const { return (positions_[index] != -1); }

    std::vector<Index>::const_iterator begin() const { return set_.begin(); }
    std::vector<Index>::const_iterator end() const { return set_.end(); }

    inline bool add(Index index);
    inline bool remove(Index index);
    inline void clear();

private:

    std::vector<Index> set_;
    std::vector<Position> positions_;

};

/******************************************************************************/

void IndexedSet::clear()
{
    for (Index index: set_)
        positions_[index] = -1;
    set_.clear();
}

inline bool IndexedSet::add(Index index)
{
    if (contains(index))
        return false;
    positions_[index] = set_.size();
    set_.push_back(index);
    return true;
}

inline bool IndexedSet::remove(Index index)
{
    if (!contains(index))
        return false;

    Position position = positions_[index];

    // Update set_
    set_[position] = set_.back();
    set_.pop_back();

    // Update positions_
    positions_[set_[position]] = position;
    positions_[index] = -1;

    return true;
}

}

