#ifndef RAW_FIELD_H
#define RAW_FIELD_H
#include "main.hpp"
#include <array>

class raw_field
{
private:
    std::array<std::array<int,FIELD_SIZE>,FIELD_SIZE> field;

    ~raw_field() = default;

public:
    void fill_row(int const row);
    void fill_col(int const col);
    void obstacle(int const num_of_obstacle, int const col, int const row);
    raw_field();
};

#endif // RAW_FIELD_H
