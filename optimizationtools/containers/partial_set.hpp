#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace optimizationtools
{

using PartialSet = int64_t;

/**
 * A partial set is a set that stores only a given subset of elements of
 * another set.
 *
 * Some elements added into a partial set might not be actually added.
 *
 * This structure is useful in dynamic programming algorithms, when not all
 * states might be stored in memeory, and therefore partial solutions are
 * stored in each state stored in memory. In this case, storing full solutions
 * in states would require too much space. Therefore, only partial solutions
 * are stored.
 *
 * The partial set is implemented as an array of bits to be as space-efficient
 * as possible.
 *
 * This class is the class that helps manipulate partial sets.
 */
class PartialSetFactory
{

public:

    typedef int64_t Index;
    typedef int64_t Position;

    /** Constructor. */
    PartialSetFactory(
            Index number_of_elements,
            int size = 64):
        positions_(number_of_elements, -1),
        elements_(size),
        size_((std::min)(64, size)) { }

    /** Add an element to the partial set factory. */
    void add_element_to_factory(
            Index index)
    {
        // If the partial set factory is full, don't add the element.
        if (current_position_ >= size_)
            return;

        positions_[index] = current_position_;
        elements_[current_position_] = index;
        current_position_++;
    }

    /** Check if a partial set contains an element. */
    bool contains(
            Index index) const
    {
        Position position = positions_[index];
        return (position != -1);
    }


    /** Check if a partial set contains an element. */
    bool contains(
            PartialSet partial_set,
            Index index) const
    {
        Position position = positions_[index];
        if (position == -1)
            return false;
        return ((partial_set >> position) & 1UL);
    }

    /** Add an element to a partial set. */
    PartialSet add(
            const PartialSet& partial_set,
            Index index) const
    {
        Position position = positions_[index];
        if (position == -1)
            return partial_set;
        return (partial_set | (1UL << position));
    }

    /** Remove an element from a partial set. */
    PartialSet remove(
            const PartialSet& partial_set,
            Index index) const
    {
        Position position = positions_[index];
        if (position == -1)
            return partial_set;
        return (partial_set & (~(1UL << position)));
    }

    /** Toggle an element of a partial set. */
    PartialSet toggle(
            const PartialSet& partial_set,
            Index index) const
    {
        Position position = positions_[index];
        if (position == -1)
            return partial_set;
        return (partial_set ^ (1UL << position));
    }

private:

    /** Positions of the elements stored of the partial sets. */
    std::vector<Position> positions_;

    /** Elements stored in the partial sets. */
    std::vector<Index> elements_;

    /** Number of elements stored in the partial sets. */
    Index size_;

    /** Current position of element to add. */
    Index current_position_ = 0;

};

}
