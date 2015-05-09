#ifndef PLACED_STONE_TYPE
#define PLACED_STONE_TYPE

// 敷地に置かれた石の情報
class placed_stone_type
{
    public:
        placed_stone_type() = default;
        ~placed_stone_type() = default;

        placed_stone_type(stone_type const & _stone,
                          point_type const & pf,
                          point_type const & ps)
        : stone(_stone), p_in_field(pf), p_in_stone(ps) {}

        stone_type const & stone;
        point_type p_in_field;
        point_type p_in_stone;
};

#endif // PLACED_STONE_TYPE

