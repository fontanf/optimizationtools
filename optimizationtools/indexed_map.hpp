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

    inline IndexedMap(Index element_number, Value null_value);
    inline virtual ~IndexedMap() { }

    inline bool empty() const { return element_number_ == 0; }
    inline Position size() const { return element_number_; }
    inline bool contains(Index index) const { return (positions_[index] < element_number_); }
    inline Value operator[](Index index) const { return ((contains(index))? elements_[positions_[index]].second: null_value_); }

    inline const_iterator begin() const { return elements_.begin(); }
    inline const_iterator end() const { return elements_.begin() + element_number_; }
    inline const_iterator out_begin() const { return elements_.begin() + element_number_; }
    inline const_iterator out_end() const { return elements_.end(); }

    inline void set(Index index, Value value);
    inline void clear() { element_number_ = 0; };

    inline bool check() const;

private:

    std::vector<std::pair<Index, Value>> elements_;
    std::vector<Position> positions_;
    Position element_number_ = 0;
    Value null_value_;

};

/******************************************************************************/

template <typename Value>
IndexedMap<Value>::IndexedMap(Index element_number, Value null_value):
    elements_(element_number),
    positions_(element_number),
    null_value_(null_value)
{
    for (Index index = 0; index < element_number; ++index) {
        elements_[index] = {index, null_value};
        positions_[index] = index;
    }
}

template <typename Value>
inline void IndexedMap<Value>::set(Index index, Value value)
{
    Position position = positions_[index];
    if (value == null_value_) { // remove
        if (position < element_number_) {
            elements_[position] = elements_[element_number_ - 1];
            elements_[element_number_ - 1] = {index, null_value_};
            positions_[elements_[position].first] = position;
            positions_[elements_[element_number_ - 1].first] = element_number_ - 1;
            element_number_--;
        }
    } else { // add
        if (position < element_number_) {
            elements_[position].second = value;
        } else {
            elements_[position] = elements_[element_number_];
            elements_[element_number_] = {index, value};
            positions_[elements_[position].first] = position;
            positions_[elements_[element_number_].first] = element_number_;
            element_number_++;
        }
    }
}

}

