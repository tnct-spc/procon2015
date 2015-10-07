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


    int upper_edge = 0; //上方から数えた障害物しかない行の数
    int right_edge = 0; //右方から数えた障害物しかない列の数
    int buttom_edge = 0;//下方から数えた障害物しかない行の数
    int left_edge = 0;  //左方から数えた障害物しかない列の数

    /*石の設置・撤去関数*/

    //制限解除(明示的に呼び出してください)
    void cancellation_of_restriction();

    /*制限解除前に使う関数*/

    //石を置けるか
    bool is_puttable_basic(stone_type const& stone, int y, int x) const;
    //石を置く
    field_type& put_stone_basic(stone_type const& stone, int y, int x);
    //石を取り除く
    field_type& remove_stone_basic();

    /*制限解除後に使う関数*/

    /**************/
    /***石を置く***/
    /**************/

    //石を置けるか
    bool is_puttable(stone_type const& stone, int y, int x) const;
    //接していなくても石を置けるか
    bool is_puttable_force(stone_type const& stone, int y, int x);
    //石を置く
    field_type& put_stone(stone_type const& stone, int y, int x);

    /******************/
    /***石を取り除く***/
    /******************/

    //一番番号の大きくて、一番最後に置いたを取り除く
    field_type& remove_large_most_number_and_just_before_stone();
    //一番番号の大きい石を取り除く
    field_type& remove_large_most_number_stone();
    //石を取り除けるか
    bool is_removable(stone_type const& stone);
    //接しているかどうか関係なく、石を取り除けるか
    bool is_removable_force(stone_type const& stone);
    //石を取り除く
    field_type& remove_stone(stone_type const& stone);

    /**************/
    /***石を探す***/
    /**************/

    //自分より若い石に接していない石を探してすべて返す
    std::vector<stone_type> search_not_in_contact_stones();
    //すべての石が自分より若い石に接しているか確認する
    bool is_stones_contact();
    //自分より若い石に接することができない(制約違反)石を探してすべて返す
    std::vector<stone_type> search_cannot_be_in_contact_stones();
    //すべての石が自分より若い石に接することができるか確認する
    bool is_stones_can_contact();


    /*その他*/

    //アクセッサ
    size_t empty_zk() const;//get_score()を同じ.なんのためにあるのかわからない
    size_t get_block_num() const;//何かが配置されている(空白以外)マスの数を返す
    size_t get_stone_num() const;
    uint64_t *get_bit_plain_field_only_stones(){
        return bit_plain_field_only_stones;
    }
    double evaluate_normalized_complexity() const;
    double evaluate_ken_o_expwy() const;
    //現在の状態における得点を返す
    size_t get_score() const;

    //置かれた石の一覧を表す配列を返す
    std::vector<stone_type> list_of_stones() const;

    // 引数の座標にある石を返す
    placed_stone_type get_stone(std::size_t const & y, std::size_t const & x);

    // bit化していない配列のアクセッサ
    raw_field_type const& get_raw_data() const;

    //マンハッタン距離が描かれたint[64][64]のフィールドを返す.引数は距離を求める最初の地点のみ1に設定したbit_field[64]
    int (*make_manhattan_field(uint64_t const bit_manhattan_start_field_[]))[64];

    // don't use this
    raw_field_type& set_raw_data();

    std::string get_answer();

    void set_random(int const obstacle, int const col, int const row);
    std::string str();
    void set_provided_stones(size_t ps);
    void print_field();

    bool get_has_limit() const;
    std::array<std::array<int, FIELD_SIZE>, FIELD_SIZE> const& route_map();
    std::vector<process_type> const& get_processes()
    {
        return processes;
    }

    // 配列の参照を返すょ
    uint64_t inline const (&get_bit_plain_field() const)[64]
    {
        return bit_plain_field;
    }

private:
    bool has_limit;

    raw_field_type raw_data;
    std::array<std::array<int, FIELD_SIZE>, FIELD_SIZE> weighted_route_map;
    std::vector<process_type> processes;
    //渡された石の総数.get_answerで必要
    size_t provided_stones;

    //is_removableで必要
    struct pair_type
    {
        int a;
        int b;
    };

    int processes_min_stone_nth();
    void init_route_map();
    bool is_placed_stone[256];
    //石が置かれているか否かを返す
    bool is_placed(stone_type const& stone) const;

        //#BitSystem
        uint64_t bit_plain_field[64];//普通のフィールド
        uint64_t bit_plain_field_only_stones[64];
        uint64_t bit_only_flame_and_obstacle_field[64];
        uint64_t bit_sides_field[64];//石のサイドフィールド
        uint64_t **bit_sides_field_at_stone_nth;//石の番号ごとのサイドフィールド
        //bool is_overlap_obstacle[39][39][2][4];//y(-7~31)-x(-7~31)-flip-angleに置いて障害物とぶつかるか<-却下
        void make_bit();//bitデータの作成
    void init_edge();
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
inline std::array<std::array<int, FIELD_SIZE>, FIELD_SIZE> const& field_type::route_map() {
    return weighted_route_map;
}
#endif // FIELD_TYPE
