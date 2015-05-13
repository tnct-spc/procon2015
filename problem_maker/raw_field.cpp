#include "raw_field.hpp"
#include <iostream>
#include <random>
#include <array>
#include <random>

raw_field::raw_field()
{
    for(auto each_row : field) each_row.fill(0);
}

void raw_field::fill_row(int const row)
{
    for(int i = FIELD_SIZE - row;i < FIELD_SIZE; i++)
    {
        for(int j = 0; j < FIELD_SIZE; j++)
        {
            field.at(i).at(j) = 1;
        }
    }
}

void raw_field::fill_col(int const col)
{
    for(int i = 0; i < FIELD_SIZE; ++i)
    {
        for(int j = FIELD_SIZE - col; j < FIELD_SIZE; j++)
        {
            field.at(i).at(j) = 1;
        }
    }
}

void raw_field::obstacle(int const num_of_obstacle, int const col, int const row)
{
    int count = 0;
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<> dist_x(0, FIELD_SIZE - row);
    std::uniform_int_distribution<> dist_y(0, FIELD_SIZE - col);
    while(count != num_of_obstacle)
    {
        int const x = dist_x(engine);
        int const y = dist_y(engine);
        if(field.at(y).at(x) == 0)
        {
            field.at(y).at(x) = 1;
            count++;
        }
    }
}
