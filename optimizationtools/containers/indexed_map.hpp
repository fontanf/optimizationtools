#pragma once

#include <vector>
#include <cstdint>
#include <random>
#include <algorithm>

namespace optimizationtools
{

template <typename Value>
class IndexedMap
{

public:

    typedef int64_t Index;
    typedef int64_t Position;
    typedef typename std::vector<std::pair<Index, Value>>::const_iterator const_iterator;

    /*
     * Constructors and destructor
     */

    /** Constructor. */
    inline IndexedMap(Index number_of_elements, Value null_value);

    /*
     * Getters
     */

    /** Return 'true' iff the map is empty. */
    inline bool empty() const { return number_of_elements_ == 0; }

    /** Get the number of elements in the map. */
    inline Position size() const { return number_of_elements_; }

    /** Retutnr 'true' iff the given element belongs to the map. */
    inline bool contains(Index index) const { return (positions_[index] < number_of_elements_); }

    /** Get the value of an element. */
    inline Value operator[](Index index) const { return ((contains(index))? elements_[positions_[index]].second: null_value_); }

    /** Get the begin iterator for elements inside the map. */
    inline const_iterator begin() const { return elements_.begin(); }

    /** Get the end iterator for elements inside the map. */
    inline const_iterator end() const { return elements_.begin() + number_of_elements_; }

    /** Get the begin iterator for elements outside the map. */
    inline const_iterator out_begin() const { return elements_.begin() + number_of_elements_; }

    /** Get the end iterator for elements outside the map. */
    inline const_iterator out_end() const { return elements_.end(); }

    /*
     * Setters
     */

    /** Set the value of an element. */
    inline void set(Index index, Value value);

    /** Remove all elements from the map. */
    inline void clear() { number_of_elements_ = 0; };

    /** Shuffle the elements of the map. */
    inline void shuffle(std::mt19937_64& generator);

    /** Shuffle the elements which belong to the map. */
    inline void shuffle_in(std::mt19937_64& generator);

    /** Shuffle a subset of elements which belong to the map. */
    inline void shuffle_in(
            Index number_of_elements,
            std::mt19937_64& generator);

    /** Shuffle the elements which don't belong to the map. */
    inline void shuffle_out(std::mt19937_64& generator);

private:

    /*
     * Private attributes
     */

    /** List of all elements, first inside then outside the map. */
    std::vector<std::pair<Index, Value>> elements_;

    /** For each element, its position in the 'elements_' vector. */
    std::vector<Position> positions_;

    /** Number of elements in the map. */
    Position number_of_elements_ = 0;

    /** Value of elements which are not in the map. */
    Value null_value_;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename Value>
IndexedMap<Value>::IndexedMap(Index number_of_elements, Value null_value):
    elements_(number_of_elements),
    positions_(number_of_elements),
    null_value_(null_value)
{
    for (Index index = 0; index < number_of_elements; ++index) {
        elements_[index] = {index, null_value};
        positions_[index] = index;
    }
}

template <typename Value>
inline void IndexedMap<Value>::set(Index index, Value value)
{
    Position position = positions_[index];
    if (value == null_value_) { // remove
        if (position < number_of_elements_) {
            elements_[position] = elements_[number_of_elements_ - 1];
            elements_[number_of_elements_ - 1] = {index, null_value_};
            positions_[elements_[position].first] = position;
            positions_[elements_[number_of_elements_ - 1].first] = number_of_elements_ - 1;
            number_of_elements_--;
        }
    } else { // add
        if (position < number_of_elements_) {
            elements_[position].second = value;
        } else {
            elements_[position] = elements_[number_of_elements_];
            elements_[number_of_elements_] = {index, value};
            positions_[elements_[position].first] = position;
            positions_[elements_[number_of_elements_].first] = number_of_elements_;
            number_of_elements_++;
        }
    }
}

template <typename Value>
inline void IndexedMap<Value>::shuffle_in(
        Index number_of_elements,
        std::mt19937_64& generator)
{
    number_of_elements = std::min(number_of_elements, number_of_elements_);
    for (Position position = 0;
            position < number_of_elements;
            ++position) {
        std::uniform_int_distribution<Position> distribution(position, number_of_elements_ - 1);
        Position position_2 = distribution(generator);
        std::swap(elements_[position], elements_[position_2]);
        positions_[elements_[position]] = position;
        positions_[elements_[position_2]] = position_2;
    }
}

template <typename Value>
inline void IndexedMap<Value>::shuffle_in(std::mt19937_64& generator)
{
    std::shuffle(elements_.begin(), elements_.begin() + number_of_elements_, generator);
    for (Position position = 0; position < number_of_elements_; ++position)
        positions_[elements_[position]] = position;
}

template <typename Value>
inline void IndexedMap<Value>::shuffle_out(std::mt19937_64& generator)
{
    std::shuffle(elements_.begin() + number_of_elements_, elements_.end(), generator);
    for (Position position = number_of_elements_; position < (Position)elements_.size(); ++position)
        positions_[elements_[position]] = position;
}

template <typename Value>
inline void IndexedMap<Value>::shuffle(std::mt19937_64& generator)
{
    std::shuffle(elements_.begin(), elements_.begin() + number_of_elements_, generator);
    std::shuffle(elements_.begin() + number_of_elements_, elements_.end(), generator);
    for (Position position = 0; position < (Position)elements_.size(); ++position)
        positions_[elements_[position]] = position;
}

}

