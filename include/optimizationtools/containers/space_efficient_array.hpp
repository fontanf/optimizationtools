#pragma once

#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <functional>
//#include <iostream>

namespace optimizationtools
{

using SpaceEfficientArray = uint64_t*;

class SpaceEfficientArrayFactory
{

public:

    using Index = int64_t;
    using Value = uint64_t;

    /** Constructor. */
    inline SpaceEfficientArrayFactory(
            Index number_of_elements,
            Value maximum_value);

    /** Get the number of elmeents. */
    inline Index number_of_elements() const { return number_of_elements_; }

    /** Get the maximum value allowed. */
    inline Value maximum_value() const { return maximum_value_; }

    /** Create a new array. */
    inline SpaceEfficientArray create_array() const;

    /** Copy an array. */
    inline SpaceEfficientArray copy_array(
            const SpaceEfficientArray& array) const;

    /** Check if two arrays are identical. */
    inline bool equal(
            const SpaceEfficientArray& array_1,
            const SpaceEfficientArray& array_2) const;

    /** Get the hash of an array. */
    inline std::size_t hash(
            const SpaceEfficientArray& array) const;

    /** Method equivalent to equal to use the class as hash structure. */
    inline bool operator()(
            const SpaceEfficientArray& array_1,
            const SpaceEfficientArray& array_2) const
    {
        return equal(array_1, array_2);
    }

    /** Method equivalent to hash to use the class as hash structure. */
    inline std::size_t operator()(
            const SpaceEfficientArray& array) const
    {
        return hash(array);
    }

    /** Set the value of an element. */
    inline void set(
            SpaceEfficientArray& array,
            Index element_id,
            Value value) const;

    /** Get the value of an element. */
    inline Value get(
            const SpaceEfficientArray& array,
            Index element_id) const;

private:

    /** Number of elements in the arrays. */
    Index number_of_elements_ = 0;

    /** Maximum value of the elements of the arrays. */
    Value maximum_value_ = 0;

    /** Number of bits for each elements. */
    std::size_t number_of_bits_for_each_element_ = 0;

    /** Real size to allocate for the arrays. */
    std::size_t array_size_ = 0;

    /** Hasher. */
    std::hash<uint64_t> hasher_;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace
{

//std::string int2bits(uint64_t value)
//{
//    std::string s;
//    for (int i = 0; i < 64; ++i)
//        s += std::to_string((value >> i) & 1);
//    return s;
//}

}

inline SpaceEfficientArrayFactory::SpaceEfficientArrayFactory(
        Index number_of_elements,
        Value maximum_value):
    number_of_elements_(number_of_elements),
    maximum_value_(maximum_value)
{
    if (maximum_value <= 0) {
        throw std::invalid_argument("Maximum value must be greater than 1.");
    }

    // Compute number_of_bits_for_each_element_.
    // 1 -> 1
    // 2 -> 2
    // 3 -> 2
    // 4 -> 3
    // 5 -> 3
    // 6 -> 3
    // 7 -> 3
    // 8 -> 4
    number_of_bits_for_each_element_ = std::floor(std::log2(maximum_value)) + 1;
    //std::cout << "number_of_bits_for_each_element_ " << number_of_bits_for_each_element_ << std::endl;

    // Compute array_size_.
    array_size_ = (number_of_elements_ * number_of_bits_for_each_element_ - 1) / (8 * sizeof(uint64_t)) + 1;
    //std::cout << "array_size_ " << array_size_ << std::endl;
}

inline SpaceEfficientArray SpaceEfficientArrayFactory::create_array() const
{
    return new uint64_t[array_size_]();
}

inline SpaceEfficientArray SpaceEfficientArrayFactory::copy_array(
        const SpaceEfficientArray& array) const
{
    SpaceEfficientArray new_array = create_array();
    for (std::size_t word_pos = 0;
            word_pos < array_size_;
            ++word_pos) {
        new_array[word_pos] = array[word_pos];
    }
    return new_array;
}

inline bool SpaceEfficientArrayFactory::equal(
        const SpaceEfficientArray& array_1,
        const SpaceEfficientArray& array_2) const
{
    for (std::size_t word_pos = 0;
            word_pos < array_size_;
            ++word_pos) {
        if (array_1[word_pos] != array_2[word_pos])
            return false;
    }
    return true;
}

inline std::size_t SpaceEfficientArrayFactory::hash(
        const SpaceEfficientArray& array) const
{
    std::size_t h = 0;
    for (std::size_t word_pos = 0;
            word_pos < array_size_;
            ++word_pos) {
        h ^= std::size_t(hasher_(array[word_pos])) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    return h;
}

inline void SpaceEfficientArrayFactory::set(
        SpaceEfficientArray& array,
        Index element_id,
        Value value) const
{
    //std::cout << "set"
    //    << " element_id " << element_id
    //    << " / " << number_of_elements()
    //    << std::endl;

    std::size_t position_start = element_id * number_of_bits_for_each_element_;
    std::size_t position_end = position_start + number_of_bits_for_each_element_ - 1;
    std::size_t word_start_pos = position_start / (8 * sizeof(uint64_t));
    std::size_t word_end_pos = position_end / (8 * sizeof(uint64_t));
    //std::cout << "position_start " << position_start << std::endl;
    //std::cout << "position_end " << position_end << std::endl;
    //std::cout << "word_start_pos " << word_start_pos
    //    << " word_end_pos " << word_end_pos
    //    << " / " << array_size_
    //    << std::endl;

    if (word_start_pos == word_end_pos) {
        //std::cout << "case 1" << std::endl;
        //        word
        // |---------------|
        //      |-------|
        // pos_start  pos_end
        std::size_t number_of_bits = position_start % (8 * sizeof(uint64_t));
        //std::cout << "number_of_bits " << number_of_bits << std::endl;
        uint64_t mask = ~((((uint64_t)1 << (number_of_bits_for_each_element_)) - 1) << number_of_bits);
        //std::cout << "mask " << int2bits(mask) << std::endl;
        uint64_t word = array[word_start_pos];
        array[word_start_pos] = (word & mask) | (value << number_of_bits);

    } else {
        //std::cout << "case 2" << std::endl;
        //     word_start      word_end
        // |---------------|---------------|
        //             |-------|
        //        pos_start  pos_end
        std::size_t number_of_bits_1 = (position_start % (8 * sizeof(uint64_t)));
        //std::cout << "number_of_bits_1 " << number_of_bits_1 << std::endl;
        uint64_t mask_start = (((uint64_t)1 << number_of_bits_1) - 1);
        //std::cout << "mask_start " << int2bits(mask_start) << std::endl;
        uint64_t word_start = array[word_start_pos];
        array[word_start_pos] = (word_start & mask_start) | (value << number_of_bits_1);

        std::size_t number_of_bits_2 = (position_end % (8 * sizeof(uint64_t))) + 1;
        //std::cout << "number_of_bits_2 " << number_of_bits_2 << std::endl;
        uint64_t mask_end = ~(((uint64_t)1 << (number_of_bits_2)) - 1);
        //std::cout << "mask_end " << int2bits(mask_end) << std::endl;
        uint64_t word_end = array[word_end_pos];
        array[word_end_pos] = (word_end & mask_end)
            | (value >> (number_of_bits_for_each_element_ - number_of_bits_2));
    }

    //for (std::size_t word_pos = 0; word_pos < array_size_; ++word_pos)
    //    std::cout << int2bits(array[word_pos]) << std::endl;
    //std::cout << std::endl;
}

inline SpaceEfficientArrayFactory::Value SpaceEfficientArrayFactory::get(
        const SpaceEfficientArray& array,
        Index element_id) const
{
    //std::cout << "get" << std::endl;
    std::size_t position_start = element_id * number_of_bits_for_each_element_;
    std::size_t position_end = position_start + number_of_bits_for_each_element_ - 1;
    std::size_t word_start_pos = position_start / (8 * sizeof(uint64_t));
    std::size_t word_end_pos = position_end / (8 * sizeof(uint64_t));

    if (word_start_pos == word_end_pos) {
        //        word
        // |---------------|
        //      |-------|
        // pos_start  pos_end
        //std::cout << "case 1" << std::endl;
        uint64_t word = array[word_start_pos];
        std::size_t number_of_bits_to_shift = position_start % (8 * sizeof(uint64_t));
        uint64_t mask = ((uint64_t)1 << (number_of_bits_for_each_element_)) - 1;
        //std::cout << "mask " << int2bits(mask) << std::endl;
        return (word >> number_of_bits_to_shift) & mask;

    } else {
        //std::cout << "case 2" << std::endl;
        //     word_start      word_end
        // |---------------|---------------|
        //             |-------|
        //        pos_start  pos_end

        std::size_t number_of_bits_1 = (position_start % (8 * sizeof(uint64_t)));
        //std::cout << "number_of_bits_1 " << number_of_bits_1 << std::endl;
        uint64_t mask_start = ~(((uint64_t)1 << number_of_bits_1) - 1);
        //std::cout << "mask_start " << int2bits(mask_start) << std::endl;
        uint64_t word_start = array[word_start_pos];

        std::size_t number_of_bits_2 = (position_end % (8 * sizeof(uint64_t))) + 1;
        //std::cout << "number_of_bits_2 " << number_of_bits_2 << std::endl;
        uint64_t mask_end = ((uint64_t)1 << (number_of_bits_2)) - 1;
        //std::cout << "mask_end " << int2bits(mask_end) << std::endl;
        uint64_t word_end = array[word_end_pos];

        return (((word_start & mask_start) >> number_of_bits_1)
                | ((word_end & mask_end) << (number_of_bits_for_each_element_ - number_of_bits_2)));
    }
}

}
