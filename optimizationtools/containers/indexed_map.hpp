#pragma once

#include <vector>
#include <cstdint>

namespace optimizationtools
{

template <typename Value>
class IndexedMap
{

public:

    typedef int64_t Index;
    typedef int64_t Position;
    typedef typename std::vector<std::pair<Index, Value>>::const_iterator const_iterator;

    inline IndexedMap(Index number_of_elements, Value null_value);
    inline virtual ~IndexedMap() { }

    inline bool empty() const { return number_of_elements_ == 0; }
    inline Position size() const { return number_of_elements_; }
    inline bool contains(Index index) const { return (positions_[index] < number_of_elements_); }
    inline Value operator[](Index index) const { return ((contains(index))? elements_[positions_[index]].second: null_value_); }

    inline const_iterator begin() const { return elements_.begin(); }
    inline const_iterator end() const { return elements_.begin() + number_of_elements_; }
    inline const_iterator out_begin() const { return elements_.begin() + number_of_elements_; }
    inline const_iterator out_end() const { return elements_.end(); }

    inline void set(Index index, Value value);
    inline void clear() { number_of_elements_ = 0; };

    inline bool check() const;

private:

    std::vector<std::pair<Index, Value>> elements_;
    std::vector<Position> positions_;
    Position number_of_elements_ = 0;
    Value null_value_;

};

/******************************************************************************/

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

}

