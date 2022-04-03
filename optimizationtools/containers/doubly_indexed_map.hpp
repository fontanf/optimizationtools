#pragma once

#include "optimizationtools/containers/indexed_set.hpp"

#include <vector>
#include <cstdint>

namespace optimizationtools
{

class DoublyIndexedMap
{

public:

    typedef int64_t Index;
    typedef int64_t Position;
    typedef int64_t Value;

    typedef typename std::vector<Index>::const_iterator const_iterator;
    typedef typename std::vector<Value>::const_iterator const_value_iterator;

    inline DoublyIndexedMap(Index number_of_elements, Value number_of_values);
    inline virtual ~DoublyIndexedMap() { }

    inline bool empty() const { return number_of_elements_ == 0; }
    inline bool contains(Index index) const { return (positions_[index].first != number_of_values_); }
    inline Value operator[](Index index) const { return positions_[index].first; }

    inline Index number_of_elements() const { return number_of_elements_; }
    inline Index number_of_elements(Value value) const { return elements_[value].size(); }
    inline const_iterator begin(Value value) const { return elements_[value].begin(); }
    inline const_iterator end(Value value) const { return elements_[value].end(); }
    inline const_iterator out_begin() const { return elements_[number_of_values_].begin(); }
    inline const_iterator out_end() const { return elements_[number_of_values_].end(); }

    inline Value number_of_values() const { return values_.size(); }
    inline const_value_iterator values_begin() const { return values_.begin(); }
    inline const_value_iterator values_end() const { return values_.end(); }

    inline void set(Index index, Value value);
    inline void unset(Index index) { set(index, number_of_values_); };

    inline bool check() const;

private:

    std::vector<std::vector<Index>> elements_;
    std::vector<std::pair<Value, Position>> positions_;
    Position number_of_elements_ = 0;
    Value number_of_values_;
    IndexedSet values_;

};

/******************************************************************************/

DoublyIndexedMap::DoublyIndexedMap(Index number_of_elements, Value number_of_values):
    elements_(number_of_values + 1),
    positions_(number_of_elements),
    number_of_values_(number_of_values),
    values_(number_of_values)
{
    elements_[number_of_values_].resize(number_of_elements);
    for (Index index = 0; index < number_of_elements; ++index) {
        positions_[index] = {number_of_values, index};
        elements_[number_of_values][index] = index;
    }
}

inline void DoublyIndexedMap::set(Index index, Value value)
{
    auto old = positions_[index];
    // Update values_.
    if (old.first != number_of_values_ && elements_[old.first].size() == 1)
        values_.remove(old.first);
    if (value != number_of_values_ && elements_[value].size() == 0)
        values_.add(value);
    // Update number_of_elements_.
    if (old.first == number_of_values_)
        number_of_elements_++;
    if (value == number_of_values_)
        number_of_elements_--;
    // Update elements_ and positions_.
    positions_[elements_[old.first].back()].second = old.second;
    elements_[old.first][old.second] = elements_[old.first].back();
    elements_[old.first].pop_back();
    positions_[index].first = value;
    positions_[index].second = elements_[value].size();
    elements_[value].push_back(index);
}

}

