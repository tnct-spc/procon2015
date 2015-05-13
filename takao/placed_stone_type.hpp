#ifndef PLACED_STONE_TYPE
#define PLACED_STONE_TYPE
#include "takao_global.hpp"
#include "stone_type.hpp"
#include "point_type.hpp"
// 敷地に置かれた石の情報
class placed_stone_type
{
    public:
    placed_stone_type() = default;
        ~placed_stone_type() = default;

        placed_stone_type(stone_type const & _stone, point_type const & pf, point_type const & ps);

        friend bool operator== (placed_stone_type const& lhs, placed_stone_type const& rhs);

        stone_type const stone;
        point_type p_in_field;
        point_type p_in_stone;
};

#endif // PLACED_STONE_TYPE

