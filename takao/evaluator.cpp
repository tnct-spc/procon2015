#include <evaluator.hpp>
#include <field_type.hpp>
#include <immintrin.h>
#include <iostream>

// fieldと石の接する辺の数÷石の辺の数
// bit version
double evaluator::normalized_contact(const field_type &field, std::vector<stone_type> const&stones, bit_process_type process) const
{
    stone_type const& stone = stones[process.nth - 1];
    uint64_t const (&field_bits)[64] = field.get_bit_plain_field();
    stone_type::bit_stones_type const& stone_bits =  stone.get_raw_bit_plain_stones();
    int const flip = process.flip;
    // static_assert(flip == 0 || flip == 1, "flip");
    int const rotate = process.get_rotate();
    // static_assert(0 <= rotate && rotate < 4, "rotate");
    int const posx = process.position.x;
    int const posy = process.position.y;

#ifdef QT_DEBUG
    if(flip != (int)stone.get_side())
        throw std::runtime_error("normalized_contact: flip does not match");
    if(rotate != stone.get_angle() / 90)
        throw std::runtime_error("normalized_contact: rotate does not match");
    if(!field.is_puttable(process.stone, posy, posx))
        throw std::runtime_error("normalized_contact: この石は敷けません 。石を敷く前のfieldを渡してね");
#endif
    uint64_t sum = 0;
    for(int i = 0; i < 8; i++) { // i行目を見る
        sum += _mm_popcnt_u64(field_bits[posy + 16 + i - 1] & stone_bits[posx + 7 + 1][flip][rotate][i]);
        sum += _mm_popcnt_u64(field_bits[posy + 16 + i + 1] & stone_bits[posx + 7 + 1][flip][rotate][i]);
        sum += _mm_popcnt_u64(field_bits[posy + 16 + i] & stone_bits[posx + 7 + 1 - 1][flip][rotate][i]);
        sum += _mm_popcnt_u64(field_bits[posy + 16 + i] & stone_bits[posx + 7 + 1 + 1][flip][rotate][i]);
    }
#ifdef QT_DEBUG
    if(sum > stone.get_side_length()) {
        stone.print_stone();
        std::cout << "sum = " << static_cast<double>(sum) << std::endl;
        std::cout << "side length = " << stone.get_side_length() << std::endl;
    }
#endif
    return static_cast<double>(sum) / stone.get_side_length();
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
int evaluator::footprint(const field_type &field, const std::vector<stone_type> &stones, bit_process_type process) const
{
    int sum = 0;
    stone_type const& stone = stones[process.nth - 1];
    stone_type::bit_stones_type const& stone_bits =  stone.get_raw_bit_plain_stones();
    int const flip = process.flip;
    int const rotate = process.get_rotate();
    int const posx = process.position.x;
    int const posy = process.position.y;
    //sum += _mm_popcnt_u64(field_bits[posy + 16 + i] & stone_bits[posx + 7 + 1][flip][rotate][i]);
    for(int i = 0; i < 8; i ++){//縦についてのループ
        u_int64_t mask = 0b1000000000000000000000000000000000000000000000000000000000000000;
        for(int j = 0; j < 64; j ++,mask >>= 1){
            if(mask & stone_bits[posx + 7 + 1][flip][rotate][i] == 1){
                //sum += route_map[i][j];
            }
        }
    }
    return sum;
}
