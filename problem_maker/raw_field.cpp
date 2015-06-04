#include "raw_field.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <array>
#include <random>

raw_field::raw_field(int const obstacle,int const col, int const row)
{
    for(auto& each_row : field) each_row.fill(0);
    std::cout << "init complete" << std::endl;
    fill_row(row);
    std::cout << "fill row complete" << std::endl;
    fill_col(col);
    std::cout << "fill col complete" << std::endl;
    place_obstacle(obstacle,col,row);
    std::cout << "place obstacle complete" << std::endl;
}

void raw_field::fill_row(int const row)
{
    for(int i = FIELD_SIZE - row; i < FIELD_SIZE; i++)
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

void raw_field::place_obstacle(int const num_of_obstacle, int const col, int const row)
{
    int count = 0;
    int insrance = 0;
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<> dist_x(0, FIELD_SIZE - row - 1);
    std::uniform_int_distribution<> dist_y(0, FIELD_SIZE - col - 1);

    while(count < num_of_obstacle)
    {
        int const x = dist_x(engine);
        int const y = dist_y(engine);
        if(field.at(y).at(x) == 0)
        {
            field.at(y).at(x) = 1;
            count++;
            if(insrance++ > FIELD_SIZE * FIELD_SIZE) return;
        }
    }
}

int raw_field::get_empty_zk()
{
    int sum = 0;
    for(auto const& each_row : field)
    {
        sum += std::count(each_row.begin(),each_row.end(),0);
    }
    return sum;
}
