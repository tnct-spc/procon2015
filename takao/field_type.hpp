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

    // copy assignment operator
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
class field_type
{
public:
    typedef std::array<std::array<int, FIELD_SIZE>, FIELD_SIZE> raw_field_type;

    field_type() = default;
    ~field_type() = default;

    field_type(std::string const & raw_field_text, size_t stones);

    int upper_edge = 0; //上方から数えた障害物しかない行の数
    int right_edge = 0; //右方から数えた障害物しかない列の数
    int buttom_edge = 0;//下方から数えた障害物しかない行の数
    int left_edge = 0;  //左方から数えた障害物しかない列の数

        field_type(std::string const & raw_field_text, size_t stones);
    //現在の状態における得点を返す
    size_t get_score();

    //石を置く  自身への参照を返す   失敗したら例外を出す
        field_type& put_stone(stone_type const& stone, int y, int x);

    //指定された場所に指定された石が置けるかどうかを返す
    bool is_puttable(stone_type const& stone, int y, int x);

        field_type& remove_just_before_stone(stone_type const& stone);
/*
    //指定された石を取り除く．その石が置かれていない場合, 取り除いた場合に不整合が生じる場合は例外を出す
    field_type& remove_stone(stone_type const& stone);

    //指定された石を取り除けるかどうかを返す
    bool is_removable(stone_type const& stone);
*/

    //置かれた石の一覧を表す配列を返す
    std::vector<stone_type> list_of_stones() const;

    //
    placed_stone_type get_stone(std::size_t const & y, std::size_t const & x);

    // accessor for raw_data
    raw_field_type const& get_raw_data() const;
    // don't use this
    raw_field_type& set_raw_data();

    std::string get_answer() const;

    void set_random(int const obstacle, int const col, int const row);
    int empty_zk();

    std::string str();

    void print_field();

private:
    raw_field_type raw_data;
    std::vector<process_type> processes;
    //渡された石の総数.get_answerで必要
    size_t provided_stones;
    //is_removableで必要
    struct pair_type
    {
        int a;
        int b;
    };

    //石が置かれているか否かを返す
    bool is_placed(stone_type const& stone);

        //#BitSystem
        uint64_t __attribute__((aligned(32))) bit_plain_field[64];//普通のフィールド
        uint64_t __attribute__((aligned(32))) bit_sides_field[64];//石の辺のフィールド
        uint64_t __attribute__((aligned(32))) bit_sides_field_just_before[256][64];//just_before石の辺のフィールド
        //bool is_overlap_obstacle[39][39][2][4];//y(-7~31)-x(-7~31)-flip-angleに置いて障害物とぶつかるか<-却下
        void make_bit();//bitデータの作成
};

#endif // FIELD_TYPE
