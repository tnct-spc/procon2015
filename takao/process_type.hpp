#ifndef PROCESS_TYPE_HPP
#define PROCESS_TYPE_HPP

#include "point_type.hpp"
#include "stone_type.hpp" // sides

struct bit_process_type
{
    unsigned int nth;
    unsigned int flip;
    unsigned int rotate;
    point_type position;

    bit_process_type() = default;
    bit_process_type(unsigned int nth_, unsigned int flip_,
                     unsigned int rotate_, point_type position_) :
        nth(nth_), flip(flip_), rotate(rotate_), position(position_)
    {
    }
    bit_process_type(unsigned int nth_, stone_type::Sides flip_,
                     unsigned int angle, point_type position_) :
        nth(nth_), flip(static_cast<int>(flip_)),
        rotate(angle / 90), position(position_)
    {
    }

    ~bit_process_type() = default;
};

#endif // PROCESS_TYPE_HPP
