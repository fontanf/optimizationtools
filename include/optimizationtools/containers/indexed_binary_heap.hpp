#pragma once

#include "optimizationtools/containers/indexed_set.hpp"

#include <vector>
#include <cstdint>
#include <functional>
#include <cassert>

namespace optimizationtools
{

template <typename Key>
class IndexedBinaryHeap
{

public:

    typedef int64_t Index;
    typedef int64_t Position;
    typedef std::function<Key (Index)> Function;

    /** Constructor. */
    IndexedBinaryHeap(Index number_of_elements = 0): heap_(1), positions_(number_of_elements + 1, -1) { }

    /** Constructor with heap initialization. */
    IndexedBinaryHeap(Index number_of_elements, Function get_key);

    /** Clear the container. */
    void clear();

    /** Reset a subset of elements. */
    void reset(const IndexedSet& indexed_set, Function get_key);

    /** Return 'true' iff the heap is empty. */
    inline bool empty() const { return heap_.size() == 1; }

    /** Get the number of elements in the heap. */
    inline Position size() const { return heap_.size() - 1; }

    /** Return 'true' iff the heap contains an element. */
    inline bool contains(Index index) { return (positions_[index] != -1);}

    /** Get the element at the top of the heap. */
    inline std::pair<Index, Key> top() const { return heap_[1]; }

    /** Get the element at a given position of the heap array. */
    inline std::pair<Index, Key> top(Position position) const { return heap_[1 + position]; }

    /** Pop the element at the top of the heap. */
    inline void pop();

    /** Update the key of an element. */
    inline void update_key(Index index, Key key);

    /** Get the key of an element. */
    inline Key key(Index index) { return heap_[positions_[index]].second; }

private:

    /*
     * Private attributes
     */

    /** Heap array. */
    std::vector<std::pair<Index, Key>> heap_;

    /**
     * Positions of each element.
     *
     * -1 if not in the heap.
     */
    std::vector<Position> positions_;

    /*
     * Private methods
     */

    inline Key cost(Position element_pos) const { return heap_[element_pos].second; }

    inline void percolate_up(Position position);

    inline void percolate_down(Position position);

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename Key>
void IndexedBinaryHeap<Key>::percolate_up(Position position)
{
    while (position != 1 && cost(position) < cost(position / 2)) {
        Position position_child = position / 2;
        positions_[heap_[position].first] = position_child;
        positions_[heap_[position_child].first] = position;
        auto tmp = heap_[position_child];
        heap_[position_child] = heap_[position];
        heap_[position] = tmp;
        position = position_child;
    }
}

template <typename Key>
void IndexedBinaryHeap<Key>::percolate_down(Position position)
{
    while (2 * position < (Position)heap_.size()) {
        Position position_child_1 = 2 * position;
        Position position_child_2 = 2 * position + 1;
        if (position_child_2 >= (Position)heap_.size()
                || cost(position_child_1) <= cost(position_child_2)) {
            if (cost(position) <= cost(position_child_1))
                return;
            positions_[heap_[position].first] = position_child_1;
            positions_[heap_[position_child_1].first] = position;
            auto tmp = heap_[position_child_1];
            heap_[position_child_1] = heap_[position];
            heap_[position] = tmp;
            position = position_child_1;
        } else {
            if (cost(position) <= cost(position_child_2))
                return;
            positions_[heap_[position].first] = position_child_2;
            positions_[heap_[position_child_2].first] = position;
            auto tmp = heap_[position_child_2];
            heap_[position_child_2] = heap_[position];
            heap_[position] = tmp;
            position = position_child_2;
        }
    }
}

template <typename Key>
IndexedBinaryHeap<Key>::IndexedBinaryHeap(Index number_of_elements, Function get_key):
    heap_(number_of_elements + 1),
    positions_(number_of_elements + 1)
{
    for (Index index = 0; index < number_of_elements; ++index) {
        heap_[index + 1] = {index, get_key(index)};
        positions_[index] = index + 1;
    }

    for (Position position = number_of_elements; position >= 1; --position)
        percolate_down(position);
}

template <typename Key>
void IndexedBinaryHeap<Key>::update_key(Index index, Key key)
{
    Position position = positions_[index];

    if (position == -1) {
        positions_[index] = heap_.size();
        heap_.push_back({index, key});
        percolate_up(heap_.size() - 1);
    } else if (key > cost(position)) {
        heap_[position].second = key;
        percolate_down(position);
    } else if (key < cost(position)) {
        heap_[position].second = key;
        percolate_up(position);
    }
}

template <typename Key>
void IndexedBinaryHeap<Key>::pop()
{
    assert(size() > 0);
    if (size() == 1) {
        positions_[heap_[1].first] = -1;
        heap_.pop_back();
    } else {
        positions_[heap_[1].first] = -1;
        positions_[heap_.back().first] = 1;
        heap_[1] = heap_.back();
        heap_.pop_back();
        percolate_down(1);
    }
}

template <typename Key>
void IndexedBinaryHeap<Key>::clear()
{
    for (auto& p: heap_)
        positions_[p.first] = -1;
    heap_.clear();
    heap_.push_back({});
}

template <typename Key>
void IndexedBinaryHeap<Key>::reset(const IndexedSet& indexed_set, Function get_key)
{
    clear();

    for (Index index: indexed_set) {
        positions_[index] = heap_.size();
        heap_.push_back({index, get_key(index)});
    }

    for (Position position = (Position)heap_.size() - 1; position >= 1; --position)
        percolate_down(position);
}

}

