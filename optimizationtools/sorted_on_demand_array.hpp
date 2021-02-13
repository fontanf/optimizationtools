#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <random>
#include <cassert>
#include <iostream>

namespace optimizationtools
{

typedef int64_t ElementId;
typedef int64_t Position;
typedef double Cost;

struct Element
{
    ElementId id;
    Cost cost;
    Position interval_start;
    Position interval_end;
};

class SortedOnDemandArray
{

public:

    SortedOnDemandArray() { }
    virtual ~SortedOnDemandArray() { }

    void reset(ElementId element_number);

    void set_cost(ElementId id, Cost cost) { elements_[id].cost = cost; }

    ElementId get(Position position, std::mt19937_64& generator);

private:

    inline void swap(Position position_1, Position position_2)
    {
        Element tmp = elements_[position_1];
        elements_[position_1] = elements_[position_2];
        elements_[position_2] = tmp;
    };

    std::vector<Element> elements_;

};

void SortedOnDemandArray::reset(ElementId element_number)
{
    elements_.clear();
    elements_.resize(element_number);
    for (Position position = 0; position < element_number; ++position) {
        elements_[position].id = position;
        elements_[position].interval_start = 0;
        elements_[position].interval_end   = element_number - 1;
    }
}

bool compare_elements(const Element& element_1, const Element& element_2)
{
    return element_1.cost < element_2.cost;
}

ElementId SortedOnDemandArray::get(Position position, std::mt19937_64& generator)
{
    assert(position >= 0);
    assert(position < (Position)elements_.size());
    while (elements_[position].interval_start != elements_[position].interval_end) {
        Position start = elements_[position].interval_start;
        Position end   = elements_[position].interval_end;
        if (end - start < 128) {
            std::sort(elements_.begin() + start, elements_.begin() + end + 1, compare_elements);
            for (Position p = start; p <= end; ++p) {
                elements_[p].interval_start = p;
                elements_[p].interval_end   = p;
            }
        } else {
            // Select pivot
            std::uniform_int_distribution<Position> distribution(start + 1, end - 1);
            Position pivot = distribution(generator);
            Cost cost_pivot = elements_[pivot].cost;

            // Partition
            swap(pivot, end);
            Position f = start;
            Position l = end;
            Position p = start;
            while (p <= l) {
                if (elements_[p].cost < cost_pivot) {
                    swap(p, f);
                    f++;
                    p++;
                } else if (elements_[p].cost > cost_pivot) {
                    swap(p, l);
                    l--;
                } else {
                    p++;
                }
            }

            // | | | | | | | | | | | | | | | | | | | | |
            //          f       p           l
            // -------|                       |-------
            // > pivot                         < pivot
            //
            for (Position p = start; p < f; ++p) {
                elements_[p].interval_start = start;
                elements_[p].interval_end   = f;
            }
            for (Position p = f; p <= l; ++p) {
                elements_[p].interval_start = p;
                elements_[p].interval_end   = p;
            }
            for (Position p = l + 1; p <= end; ++p) {
                elements_[p].interval_start = l + 1;
                elements_[p].interval_end   = end;
            }
        }
    }
    return elements_[position].id;
}

}

