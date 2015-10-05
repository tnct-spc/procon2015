#ifndef FIELD_TYPE
#define FIELD_TYPE
#include "stone_type.hpp"
#include "point_type.hpp"
#include "placed_stone_type.hpp"
#include <array>
#include <string>
#include <vector>
#include <bitset>

int constexpr FIELD_SIZE = 32;

// 解答データの手順ひとつ分
class process_type
{
public:
    process_type() = delete;
    ~process_type() = default;

    process_type(stone_type const & _stone,
                 point_type const & _position)
    : stone(_stone), position(_position) {}

    process_type & operator=(process_type const & other)
    {
        const_cast<stone_type &>(stone) = other.stone;
        const_cast<point_type &>(position) = other.position;
        return *this;
    }

    stone_type const stone;
    point_type const position;
};

// 敷地
//field_type&が返り値の関数は*thisの参照が返り値です
class field_type
{
public:
    typedef std::array<std::array<int, FIELD_SIZE>, FIELD_SIZE> raw_field_type;

    field_type() = default;
    field_type(std::string const & raw_field_text, size_t stones);
    field_type(const int obstacles, const int cols, const int rows);

    ~field_type() = default;

    /*石の設置・撤去関数*/

    //石を置けるか
    bool is_puttable_basic(stone_type const& stone, int y, int x) const;
    //石を置く
    field_type& put_stone_basic(stone_type const& stone, int y, int x);
    //石を取り除く
    field_type& remove_stone_basic();
    /*その他*/

    //アクセッサ
    size_t empty_zk() const;//get_score()を同じ.なんのためにあるのかわからない
    size_t get_block_num() const;//何かが配置されている(空白以外)マスの数を返す
    size_t get_stone_num() const;
    double evaluate_normalized_complexity() const;

    //現在の状態における得点を返す
    size_t get_score() const;

    //置かれた石の一覧を表す配列を返す
    std::vector<stone_type> list_of_stones() const;

    // don't use this
    raw_field_type& set_raw_data();

    std::string get_answer();

    void set_random(raw_field_type raw_data,int const obstacle, int const col, int const row);
    std::string str();
    void set_provided_stones(size_t ps);
    void print_field();

    // 配列の参照を返すょ
    uint64_t inline const (&get_bit_plain_field() const)[64]
    {
        return bit_plain_field;
    }

private:
    std::vector<process_type> processes;
    //渡された石の総数.get_answerで必要
    size_t provided_stones;
    bool is_placed_stone[256];
    //石が置かれているか否かを返す
    bool is_placed(stone_type const& stone) const;

    uint64_t bit_plain_field[64];//普通のフィールド
    uint64_t bit_sides_field[64];//石のサイドフィールド
    uint64_t bit_sides_field_just_before[256][64];
    void make_bit(const raw_field_type raw_data);//bitデータの作成
};
inline size_t field_type::empty_zk() const{
    return get_score();
}
inline size_t field_type::get_block_num() const
{
    return (FIELD_SIZE * FIELD_SIZE) - get_score();
}
inline size_t field_type::get_stone_num() const{
    return processes.size();
}
#endif // FIELD_TYPE
