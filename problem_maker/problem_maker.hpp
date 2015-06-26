#ifndef PROBLEM_MAKER_HPP
#define PROBLEM_MAKER_HPP
#include <array>

int static constexpr STONE_SIZE = 8;
int static constexpr FIELD_SIZE = 32;

typedef std::array<std::array<int,STONE_SIZE>,STONE_SIZE> raw_stone_type;
typedef std::array<std::array<int,FIELD_SIZE>,FIELD_SIZE> raw_field_type;

#endif // PROBLEM_MAKER_HPP
