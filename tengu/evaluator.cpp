#include <evaluator.hpp>
#include <field_type.hpp>
#include <immintrin.h>

// fieldと石の接する辺の数÷石の辺の数
double evaluator::normalized_contact(field_type const& field, process_type const& process) const
{
    uint64_t const (&field_bits)[64] = field.get_bit_plain_field();
    stone_type::bit_stones_type const& stone_bits =  process.stone.get_raw_bit_plain_stones();
    int const flip = (int)process.stone.get_side();
    // static_assert(flip == 0 || flip == 1, "flip");
    int const rotate = process.stone.get_angle() / 90;
    // static_assert(0 <= rotate && rotate < 4, "rotate");
    int const posx = process.position.x;
    int const posy = process.position.y;

    int sum = 0;
    for(int i = 0; i < 8; i++) {
        // upper
        sum += _mm_popcnt_u64(field_bits[posy + i - 1] & stone_bits[posx + 1][flip][rotate][i]);
        // under
        sum += _mm_popcnt_u64(field_bits[posy + i + 1] & stone_bits[posx + 1][flip][rotate][i]);
        // left
        sum += _mm_popcnt_u64(field_bits[posy + i] & stone_bits[posx ][flip][rotate][i]);
        // right
        sum += _mm_popcnt_u64(field_bits[posy + i] & stone_bits[posx + 2][flip][rotate][i]);
    }
    /*
    if(sum > process.stone.get_side_length())
    {
        process.stone.print_stone();
        std::cout << "sum = " << static_cast<double>(sum) << std::endl;
        std::cout << "side length = " << process.stone.get_side_length() << std::endl;
    }
    */
    return static_cast<double>(sum) / process.stone.get_side_length();
}

int evaluator::nextbranches(const field_type &field, stone_type &stone) const
{
    // 全点でのis_puttableの回数を数える
    // 重そう
    int sum = 0;
    for (int y = -7; y < 32; y++) {
        for (int x = -7; x < 32; x++) {
            sum += field.is_puttable_basic(stone, y, x) // 0
                 + field.is_puttable_basic(stone.rotate(90), y, x) // 90
                 + field.is_puttable_basic(stone.rotate(90), y, x) // 180
                 + field.is_puttable_basic(stone.rotate(90), y, x) // 270
                 + field.is_puttable_basic(stone.rotate(90).flip(), y, x) // 0f
                 + field.is_puttable_basic(stone.rotate(90), y, x) // 90f
                 + field.is_puttable_basic(stone.rotate(90), y, x) // 180f
                 + field.is_puttable_basic(stone.rotate(90), y, x); // 270f
            stone.rotate(90).flip(); // 0
        }
    }
    return sum;
}
