#ifndef RAW_STONE_HPP
#define RAW_STONE_HPP
#include <vector>
#include <array>

int static constexpr STONE_SIZE = 8;
typedef std::array<std::array<int,STONE_SIZE>,STONE_SIZE> raw_stone_type;

class raw_stone
{
private:
    enum class direction { UP, RIGHT, DOWN, LEFT };

public:
    std::vector<raw_stone_type> data;

    raw_stone(int const algo,int const field_zk);
    ~raw_stone() = default;

    void simple_create(int const field_zk);
    int get_fill_zk(raw_stone_type stone);
    void print_stone(std::size_t const nth);
    void print_stone(raw_stone_type const& stone);
    bool is_hole(raw_stone_type& stone, int zk);

};

#endif // RAW_STONE_HPP
