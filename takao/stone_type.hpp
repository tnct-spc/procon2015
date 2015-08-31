#ifndef STONE_TYPE
#define STONE_TYPE
#include "point_type.hpp"
#include <array>
#include <vector>

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

        stone_type() = default;
        ~stone_type() = default;

        stone_type(std::string const & raw_stone_text, int const _nth);

        friend bool operator== (stone_type const& lhs, stone_type const& rhs);

        int at(size_t y,size_t x);
        int const & at(size_t y,size_t x) const;
        raw_stone_type const & get_raw_data() const;
        stone_type& rotate(int angle);
        stone_type& flip();
        size_t get_area() const;
        stone_type::Sides get_side() const;
        std::size_t get_angle() const;
        int get_nth()const;
        int get_side_length()const;
        corner_type get_corner();
        int count_n_row(int const n)const;
        int count_n_col(int const n)const;
        uint64_t get_bit_plain_stones(int x,int flip,int rotate,int y) const;


    private:
        int  nth;
        std::array<raw_stone_type,STONE_SIZE>  raw_data_set;
        Sides current_side = Sides::Head;
        std::size_t current_angle = 0;

        raw_stone_type _rotate(raw_stone_type const & raw_data, int angle);
        raw_stone_type _flip(raw_stone_type stone);

        //#BitSystem
        //石のマスク(int64_t bit_stones[41][2][4][8];)
        std::vector<std::vector<std::vector<std::vector<uint64_t>>>> bit_plain_stones = std::vector<std::vector<std::vector<std::vector<uint64_t>>>>(41,std::vector<std::vector<std::vector<uint64_t>>>(2,std::vector<std::vector<uint64_t>>(4,std::vector<uint64_t>(8))));

        //bitデータの作成
        void make_bit();
};



#endif // STONE_TYPE
