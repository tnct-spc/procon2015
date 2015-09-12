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

    ~field_type() = default;
    //アクセッサ
    size_t get_score();//現在の状態の得点を返す
    size_t empty_zk();//get_score()を同じ.なんのためにあるのかわからない
    size_t get_block_num();//何かが配置されている(空白以外)マスの数を返す
    raw_field_type const& get_raw_data() const;//bit化していない配列のアクセッサ
    placed_stone_type get_stone(std::size_t const & y, std::size_t const & x);//引数の座標にある石を返す
    std::string get_answer() const;
    std::vector<stone_type> list_of_stones() const;//置かれた石の一覧を表す配列を返す
    double evaluate_normalized_complexity();

    //石の操作
    field_type& put_stone(stone_type const& stone, int y, int x);//失敗したら例外を出す
    bool is_puttable(stone_type const& stone, int y, int x);//指定された場所に指定された石が置けるかどうか
    field_type& remove_just_before_stone(stone_type const& stone);//１手前の石を取り除く
    /*bit化で一時的に利用不能
    //field_type& remove_stone(stone_type const& stone);//指定された石を取り除く．その石が置かれていない場合, 取り除いた場合に不整合が生じる場合は例外を出す
    //bool is_removable(stone_type const& stone);//指定された石を取り除けるかどうかを返す
    */

    //謎
    void set_random(int const obstacle, int const col, int const row);
    std::string str();
    void print_field();

private:
    raw_field_type raw_data;
    uint64_t bit_plain_field[64];//普通のフィールド
    uint64_t bit_sides_field[64];//石の辺のフィールド
    uint64_t bit_sides_field_just_before[256][64];//１手前石の辺のフィールド
    std::vector<process_type> processes;
    size_t provided_stones;//渡された石の総数.get_answerで必要
    int upper_edge = 0; //上方から数えた障害物しかない行の数
    int right_edge = 0; //右方から数えた障害物しかない列の数
    int buttom_edge = 0;//下方から数えた障害物しかない行の数
    int left_edge = 0;  //左方から数えた障害物しかない列の数

    struct pair_type//is_removableで必要
    {
        int a;
        int b;
    };

    bool is_placed(stone_type const& stone);//石が置かれているか否かを返す
    void make_bit();//bitデータの作成
};
inline size_t field_type::empty_zk(){
    return get_score();
}
inline size_t field_type::get_block_num(){
    return (FIELD_SIZE * FIELD_SIZE) - get_score();
}

#endif // FIELD_TYPE
