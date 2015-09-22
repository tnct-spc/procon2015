#include <evaluator.hpp>
#include <field_type.hpp>
#include <immintrin.h>

// fieldと石の接する辺の数÷石の辺の数
double evaluator::normalized_contact(const field_type &field, const process_type &process)
{
    // これから書く
    uint64_t const (&field_bits)[64] = field.bit_plain_field;
    stone_type::bit_stones_type const& stone_bits = process.stone.bit_plain_stones;
    int sum;
    sum += _mm_popcnt_u64(~);
    return static_cast<double>(sum) / process.stone.sides;
}

int evaluator::nextbranches(const field_type &field, const stone_type &stone)
{
    // 全点でのis_puttableの回数を数える
}
