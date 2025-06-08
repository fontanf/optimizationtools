#pragma once

#include <vector>
#include <cstdint>
#include <random>
#include <algorithm>
#include <stdexcept>

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

    /*
     * Constructors and destructor
     */

    /** Constructor. */
    inline IndexedSet(Index number_of_elements);

    /*
     * Getters
     */

    /** Return 'true' iff the set is empty. */
    inline bool empty() const { return number_of_elements_ == 0; }

    /** Get the number of elements in the set. */
    inline Position size() const { return number_of_elements_; }

    /** Retutnr 'true' iff the given element belongs to the set. */
    inline bool contains(Index index) const { return (positions_[index] < number_of_elements_); }

    /**
     * Return 'true' iff all the elements from a given vector belong to the
     * set.
     */
    inline bool contains(const std::vector<Index>& indices) const
    {
        for (Index index: indices)
            if (!contains(index))
                return false;
        return true;
    }

    /** Get the position of an element in the set. */
    inline Position position(Index index) const { return positions_[index]; }

    /** Get the begin iterator for elements inside the set. */
    inline std::vector<Index>::const_iterator begin() const { return elements_.begin(); }

    /** Get the end iterator for elements inside the set. */
    inline std::vector<Index>::const_iterator end() const { return elements_.begin() + number_of_elements_; }

    /** Get the begin iterator for elements outside the set. */
    inline std::vector<Index>::const_iterator out_begin() const { return elements_.begin() + number_of_elements_; }

    /** Get the end iterator for elements outside the set. */
    inline std::vector<Index>::const_iterator out_end() const { return elements_.begin() + size_; }

    /*
     * Setters
     */

    /** Add an element to the set. */
    inline bool add(Index index);

    /** Remove an element from the set. */
    inline bool remove(Index index);

    /** Remove all elements from the set. */
    inline void clear() { number_of_elements_ = 0; }

    /** Add all elements to the set. */
    inline void fill() { number_of_elements_ = size_; }

    /** Resize the set. */
    inline void resize_and_clear(Position number_of_elements);

    /** Shuffle the elements of the set. */
    inline void shuffle(std::mt19937_64& generator);

    /** Shuffle the elements which belong to the set. */
    inline void shuffle_in(std::mt19937_64& generator);

    /** Shuffle a subset of elements which belong to the set. */
    inline void shuffle_in(
            Index number_of_elements,
            std::mt19937_64& generator);

    /** Shuffle the elements which don't belong to the set. */
    inline void shuffle_out(std::mt19937_64& generator);

private:

    /*
     * Private attributes
     */

    /** List of all elements, first inside then outside the set. */
    std::vector<Index> elements_;

    /** Total number of elements (in and out of the set). */
    Position size_ = 0;

    /** For each element, its position in the 'elements_' vector. */
    std::vector<Position> positions_;

    /** Number of elements in the set. */
    Position number_of_elements_ = 0;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline IndexedSet::IndexedSet(Index number_of_elements):
    elements_(number_of_elements),
    size_(number_of_elements),
    positions_(number_of_elements)
{
    for (Index index = 0; index < number_of_elements; ++index) {
        elements_[index] = index;
        positions_[index] = index;
    }
}

inline bool IndexedSet::add(Index index)
{
    //if (index < 0 || index >= size_) {
    //    throw std::invalid_argument(
    //            "optimizationtools::IndexedSet::add: "
    //            "out-of-bound index; "
    //            "index: " + std::to_string(index) + "; "
    //            "size_: " + std::to_string(size_) + ".");
    //}

    Position position = positions_[index];
    if (position < number_of_elements_)
        return false;
    elements_[position] = elements_[number_of_elements_];
    elements_[number_of_elements_] = index;
    positions_[elements_[position]] = position;
    positions_[elements_[number_of_elements_]] = number_of_elements_;
    number_of_elements_++;
    return true;
}

inline bool IndexedSet::remove(Index index)
{
    //if (index < 0 || index >= size_) {
    //    throw std::invalid_argument(
    //            "optimizationtools::IndexedSet::remove: "
    //            "out-of-bound index; "
    //            "index: " + std::to_string(index) + "; "
    //            "size_: " + std::to_string(size_) + ".");
    //}

    Position position = positions_[index];
    if (position >= number_of_elements_)
        return false;
    elements_[position] = elements_[number_of_elements_ - 1];
    elements_[number_of_elements_ - 1] = index;
    positions_[elements_[position]] = position;
    positions_[elements_[number_of_elements_ - 1]] = number_of_elements_ - 1;
    number_of_elements_--;
    return true;
}

inline void IndexedSet::resize_and_clear(
        Position number_of_elements)
{
    if (number_of_elements < 0 || number_of_elements > elements_.size()) {
        throw std::invalid_argument(
                "optimizationtools::IndexedSet::resize_and_clear: "
                "'number_of_elements' is too large; "
                "number_of_elements: " + std::to_string(number_of_elements) + "; "
                "elements_.size(): " + std::to_string(elements_.size()) + "." );
    }

    for (Index index = number_of_elements;
            index < size_;
            ++index) {
        Position position = positions_[index];
        if (position == index)
            continue;
        elements_[position] = elements_[index];
        elements_[index] = index;
        positions_[elements_[position]] = position;
        positions_[index] = index;
    }
    size_ = number_of_elements;
    clear();
}

inline void IndexedSet::shuffle_in(
        Index number_of_elements,
        std::mt19937_64& generator)
{
    number_of_elements = std::min(number_of_elements, number_of_elements_);
    for (Position position = 0;
            position < number_of_elements;
            ++position) {
        std::uniform_int_distribution<Position> distribution(
                position,
                number_of_elements_ - 1);
        Position position_2 = distribution(generator);
        std::swap(elements_[position], elements_[position_2]);
        positions_[elements_[position]] = position;
        positions_[elements_[position_2]] = position_2;
    }
}

inline void IndexedSet::shuffle_in(std::mt19937_64& generator)
{
    std::shuffle(
            elements_.begin(),
            elements_.begin() + number_of_elements_,
            generator);
    for (Position position = 0; position < number_of_elements_; ++position)
        positions_[elements_[position]] = position;
}

inline void IndexedSet::shuffle_out(std::mt19937_64& generator)
{
    std::shuffle(
            elements_.begin() + number_of_elements_,
            elements_.begin() + size_,
            generator);
    for (Position position = number_of_elements_;
            position < size_;
            ++position) {
        positions_[elements_[position]] = position;
    }
}

inline void IndexedSet::shuffle(std::mt19937_64& generator)
{
    std::shuffle(
            elements_.begin(),
            elements_.begin() + number_of_elements_,
            generator);
    std::shuffle(
            elements_.begin() + number_of_elements_,
            elements_.begin() + size_,
            generator);
    for (Position position = 0;
            position < size_;
            ++position) {
        positions_[elements_[position]] = position;
    }
}

}
