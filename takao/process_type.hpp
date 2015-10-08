#ifndef PROCESS_TYPE_HPP
#define PROCESS_TYPE_HPP

#include "point_type.hpp"
#include "stone_type.hpp" // sides

struct bit_process_type
{
    unsigned int nth;
    unsigned int flip;
    unsigned int angle;
    point_type position;

    bit_process_type(unsigned int nth_, unsigned int flip_,
                     unsigned int angle_, point_type position_) :
        nth(nth_), flip(flip_), angle(angle_), position(position_)
    {
    }
    bit_process_type(){}

    int inline get_rotate() const // f**k
    {
        return angle / 90;
    }
};

#endif // PROCESS_TYPE_HPP
