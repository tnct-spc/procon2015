#include "placed_stone_type.hpp"
#include <functional>

placed_stone_type::placed_stone_type(stone_type const & _stone, point_type const & pf, point_type const & ps)
                   : stone(_stone), p_in_field(pf), p_in_stone(ps)
{

}

placed_stone_type::placed_stone_type():stone(stone_type()){}

bool operator== (placed_stone_type const& lhs, placed_stone_type const& rhs)
{
    return lhs.stone == rhs.stone && lhs.p_in_field == rhs.p_in_field && lhs.p_in_stone == rhs.p_in_stone;
}
