#pragma once

#include "optimizationtools/indexed_set.hpp"

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

    inline DoublyIndexedMap(Index element_number, Value value_number);
    inline virtual ~DoublyIndexedMap() { }

    inline bool empty() const { return element_number_ == 0; }
    inline bool contains(Index index) const { return (positions_[index].first != value_number_); }
    inline Value operator[](Index index) const { return positions_[index].first; }

    inline Index element_number() const { return element_number_; }
    inline Index element_number(Value value) const { return elements_[value].size(); }
    inline const_iterator begin(Value value) const { return elements_[value].begin(); }
    inline const_iterator end(Value value) const { return elements_[value].end(); }
    inline const_iterator out_begin() const { return elements_[value_number_].begin(); }
    inline const_iterator out_end() const { return elements_[value_number_].end(); }

    inline Value value_number() const { return values_.size(); }
    inline const_value_iterator values_begin() const { return values_.begin(); }
    inline const_value_iterator values_end() const { return values_.end(); }

    inline void set(Index index, Value value);
    inline void unset(Index index) { set(index, value_number_); };

    inline bool check() const;

private:

    std::vector<std::vector<Index>> elements_;
    std::vector<std::pair<Value, Position>> positions_;
    Position element_number_ = 0;
    Value value_number_;
    IndexedSet values_;

};

/******************************************************************************/

DoublyIndexedMap::DoublyIndexedMap(Index element_number, Value value_number):
    elements_(value_number + 1),
    positions_(element_number),
    value_number_(value_number),
    values_(value_number)
{
    elements_[value_number_].resize(element_number);
    for (Index index = 0; index < element_number; ++index) {
        positions_[index] = {value_number, index};
        elements_[value_number][index] = index;
    }
}

inline void DoublyIndexedMap::set(Index index, Value value)
{
    auto old = positions_[index];
    // Update values_.
    if (old.first != value_number_ && elements_[old.first].size() == 1)
        values_.remove(old.first);
    if (value != value_number_ && elements_[value].size() == 0)
        values_.add(value);
    // Update element_number_.
    if (old.first == value_number_)
        element_number_++;
    if (value == value_number_)
        element_number_--;
    // Update elements_ and positions_.
    positions_[elements_[old.first].back()].second = old.second;
    elements_[old.first][old.second] = elements_[old.first].back();
    elements_[old.first].pop_back();
    positions_[index].first = value;
    positions_[index].second = elements_[value].size();
    elements_[value].push_back(index);
}

}

