#ifndef RAW_FIELD_HPP
#define RAW_FIELD_HPP
#include "problem_maker.hpp"
#include <array>

class raw_field
{
public:
    raw_field_type field;

    void fill_row(int const row);
    void fill_col(int const col);
    void place_obstacle(int const num_of_obstacle, int const col, int const row);
    int get_empty_zk();

    raw_field(int const obstacle,int const col, int const row);
    ~raw_field() = default;
};

#endif // RAW_FIELD_HPP
