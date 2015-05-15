#ifndef RAW_STONE_HPP
#define RAW_STONE_HPP
#include "problem_maker.hpp"

class raw_stone
{
private:
    std::array<std::array<bool, STONE_SIZE>, STONE_SIZE> data;
    enum class direction { UP, RIGHT, DOWN, LEFT };

public:
    raw_stone();
    ~raw_stone() = default;
    void create(int zk);
    void create();

};

#endif // RAW_STONE_H
