#ifndef RAW_STONE_HPP
#define RAW_STONE_HPP
#include "problem_maker.hpp"
#include <vector>
#include <array>

class raw_stone
{
private:
    enum class direction { UP, RIGHT, DOWN, LEFT };

public:
    std::vector<raw_stone_type> data;

    raw_stone(int const algo,int const field_zk);
    ~raw_stone() = default;

    void simple_create(int const field_zk);
    int get_fill_zk(raw_stone_type stone);
    void print_stone(std::size_t const nth);
    void print_stone(raw_stone_type const& stone);
    bool is_hole(raw_stone_type& stone, int zk);

};

#endif // RAW_STONE_H
