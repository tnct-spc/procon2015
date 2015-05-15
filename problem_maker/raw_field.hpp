#ifndef RAW_FIELD_HPP
#define RAW_FIELD_HPP
#include "problem_maker.hpp"
#include <array>

class raw_field
{
private:
    raw_field_type field;
    ~raw_field() = default;

public:
    void fill_row(int const row);
    void fill_col(int const col);
    void obstacle(int const num_of_obstacle, int const col, int const row);
    raw_field();
};

#endif // RAW_FIELD_HPP
