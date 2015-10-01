#ifndef STONE_TYPE
#define STONE_TYPE
#include <vector>
#include "stone_type.hpp"
#include "point_type.hpp"
#include "utils.hpp"
#include "point_type.hpp"
#include <stdexcept>
#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
//#include <sstream>

int constexpr STONE_SIZE = 8;
struct corner_type
{
    int value;
    point_type point;
};

// 石
class stone_type
{
public:
    enum struct Sides {Head = 0, Tail = 1};
    typedef std::array<std::array<int,STONE_SIZE>,STONE_SIZE> raw_stone_type;
        //以前はstd::vector<std::vector<std::vector<std::vector<uint64_t>>>>でした;
        //typedef std::array <std::array <std::array <std::array <uint64_t,8>,4>,2>,41> __attribute__((aligned(32))) bit_stones_type;
        typedef uint64_t bit_stones_type[41][2][4][8];
    stone_type() = default;
    stone_type(int const zk); // set_random
    stone_type(int const zk,int const nth);
    ~stone_type() = default;

    stone_type(std::string const & raw_stone_text, int const _nth);

    friend bool operator== (stone_type const& lhs, stone_type const& rhs);

    int at(size_t y,size_t x);
    int const & at(size_t y,size_t x) const;
    raw_stone_type const & get_raw_data() const;
    stone_type& rotate(int angle);
    stone_type& flip();
    stone_type& set_angle(int angle);
    stone_type& set_side(stone_type::Sides side);
    size_t get_area() const;
    stone_type::Sides get_side() const;
    std::size_t get_angle() const;
    int get_nth()const;
    int get_side_length()const;
    corner_type get_corner();
    int count_n_row(int const n)const;
    int count_n_col(int const n)const;
    std::string str() const;
        uint64_t const&get_bit_plain_stones(int x, int flip, int rotate, int y) const;
        bit_stones_type const& get_raw_bit_plain_stones() const;
    void print_stone() const;
    void init_stone();
private:
    int nth;
    std::size_t side_length = 0;
    std::size_t area;
    std::array<raw_stone_type,STONE_SIZE>  raw_data_set;
    Sides current_side = Sides::Head;
    std::size_t current_angle = 0;

    raw_stone_type _rotate(raw_stone_type const & raw_data, int angle);
    raw_stone_type _flip(raw_stone_type stone);
    /* from raw_stone */
    enum class direction { UP, RIGHT, DOWN, LEFT };
    bool _has_hole(raw_stone_type stone);
    void _set_random(int zk);
    void _set_from_raw(raw_stone_type raw);
    bool inline _is_in_stone(int p);

        //#BitSystem
        //石のマスク(int64_t bit_stones[41][2][4][8];)
        bit_stones_type bit_plain_stones;
        //bitデータの作成
        void make_bit();
    bool inline _is_in_stone(int y, int x);
    bool inline _is_in_stone(point_type p);

};

inline bool operator== (stone_type const& lhs, stone_type const& rhs)
{
    return lhs.get_raw_data() == rhs.get_raw_data();
}

//生配列へのアクセサ
//座標を受け取ってそこの値を返す
inline int const & stone_type::at(size_t y,size_t x) const
{
    return get_raw_data().at(y).at(x);
}

inline int stone_type::at(size_t y,size_t x)
{
    return get_raw_data().at(y).at(x);
}

//石へのアクセサ
//生配列への参照を返す
inline stone_type::raw_stone_type const& stone_type::get_raw_data() const
{
    return raw_data_set.at(static_cast<unsigned>(current_side)*4 + current_angle / 90);
}

//時計回りを正方向として指定された角度だけ回転する
// 自身への参照を返す
inline stone_type& stone_type::rotate(int angle)
{
    current_angle = (current_angle + angle) % 360;
    return *this;
}
inline stone_type& stone_type::set_angle(int angle){
    current_angle = angle;
    return *this;
}

//左右に反転する
//自身への参照を返す
inline stone_type& stone_type::flip()
{
    current_side = current_side == Sides::Head ? Sides::Tail : Sides::Head;
    return *this;
}
inline stone_type& stone_type::set_side(stone_type::Sides side){
    current_side = side;
    return *this;
}

//面積を返す
inline size_t stone_type::get_area() const
{
    return area;
}

//現在の表裏を返す
inline stone_type::Sides stone_type::get_side() const
{
    return current_side;
}

//現在の角度を返す
inline std::size_t stone_type::get_angle() const
{
    return current_angle;
}

//n列目のブロック数を返す
inline int stone_type::count_n_row(int const n)const
{
    int const current_data = static_cast<unsigned>(current_side)*4 + current_angle / 90;
    return std::count(raw_data_set.at(current_data).at(n).begin(),raw_data_set.at(current_data).at(n).end(),1);
}

//n行目のブロック数を返す
inline int stone_type::count_n_col(int const n)const
{
    int const current_data = static_cast<unsigned>(current_side)*4 + current_angle / 90;
    int sum = 0;
    for(int i = 0;i < STONE_SIZE; ++i) sum += raw_data_set.at(current_data).at(i).at(n);
    return sum;
}


//左右に反転する
inline stone_type::raw_stone_type stone_type::_flip(raw_stone_type stone)
{
    for(auto& each_stone:stone)
        std::reverse(each_stone.begin(),each_stone.end());
    return stone;
}

inline int stone_type::get_nth()const
{
    return nth;
}

inline int stone_type::get_side_length() const
{
    return side_length;
}

inline uint64_t const& stone_type::get_bit_plain_stones(int x, int flip, int rotate, int y) const
{
    //return bit_plain_stones.at(x+1).at(flip).at(rotate).at(y);
    return bit_plain_stones[x+1][flip][rotate][y];
}

inline const stone_type::bit_stones_type &stone_type::get_raw_bit_plain_stones() const{
    return bit_plain_stones;
}
/* returns if (y, x) is in stone (valid range) */
inline bool stone_type::_is_in_stone(int p)
{
    return 0 <= p && p < STONE_SIZE;
}

inline bool stone_type::_is_in_stone(int y, int x)
{
    return _is_in_stone(x) && _is_in_stone(y);
}

inline bool stone_type::_is_in_stone(point_type p)
{
    return _is_in_stone(p.y, p.x);
}

#endif // STONE_TYPE
