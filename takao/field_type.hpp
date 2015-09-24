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

    /**************/
    /***石を置く***/
    /**************/

    //石を置けるか
    bool is_puttable(stone_type const& stone, int y, int x);
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
    //障害物にぶつかるか
    bool is_touch_flame(stone_type const& stone, int y, int x);

    /*************/
    /***その他****/
    /*************/

    //アクセッサ
    size_t empty_zk();//get_score()を同じ.なんのためにあるのかわからない
    size_t get_block_num();//何かが配置されている(空白以外)マスの数を返す
    double evaluate_normalized_complexity();

    //現在の状態における得点を返す
    size_t get_score() const;

    //置かれた石の一覧を表す配列を返す
    std::vector<stone_type> list_of_stones() const;

    //
    placed_stone_type get_stone(std::size_t const & y, std::size_t const & x);

    // accessor for raw_data
    raw_field_type const& get_raw_data() const;
    // don't use this
    raw_field_type& set_raw_data();

    std::string get_answer();

    void set_random(int const obstacle, int const col, int const row);
    std::string str();
    void set_provided_stones(size_t ps);
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

    int processes_min_stone_nth();

    bool is_placed_stone[256];
    //石が置かれているか否かを返す
    bool is_placed(stone_type const& stone);

        //#BitSystem
        uint64_t bit_plain_field[64];//普通のフィールド
        uint64_t bit_flame_field[64];//障害物のみのフィールド
        uint64_t bit_sides_field[64];//石のサイドフィールド
        uint64_t bit_sides_field_at_stone_nth[257][64];//石の番号ごとのサイドフィールド
        //bool is_overlap_obstacle[39][39][2][4];//y(-7~31)-x(-7~31)-flip-angleに置いて障害物とぶつかるか<-却下
        void make_bit();//bitデータの作成
    void init_edge();
};
inline size_t field_type::empty_zk(){
    return get_score();
}
inline size_t field_type::get_block_num(){
    return (FIELD_SIZE * FIELD_SIZE) - get_score();
}

#endif // FIELD_TYPE
