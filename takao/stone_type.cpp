//#define _DEBUG

#ifdef _DEBUG
#include <QDebug>
#include <iostream>
#include <bitset>
#endif

#include "stone_type.hpp"
#include "point_type.hpp"
#include "utils.hpp"
#include "point_type.hpp"
#include <stdexcept>
#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <sstream>
namespace std
{
    ostream& operator << (ostream& os, stone_type::Sides const& sides)
    {
        if(sides == stone_type::Sides::Head) os << "Head";
        else os << "Tail";
        return os;
    }
}


stone_type::stone_type(std::string const & raw_stone_text, int const _nth) :nth(_nth)
{
    auto rows = _split(raw_stone_text, "\r\n");
    for (std::size_t i = 0; i < raw_data_set.at(0).size(); ++i) {
        std::transform(rows[i].begin(), rows[i].end(), raw_data_set.at(0)[i].begin(),
                       [](auto const & c) { return c == '1'; });
    }
    init_stone();
}

stone_type::stone_type(int const zk)
{
    _set_random(zk);
    init_stone();
}
stone_type::stone_type(int const zk, int const nth) : nth(nth)
{
    _set_random(zk);
    init_stone();
}
void stone_type::init_stone()
{
    //rotate用のarrayを準備する
    std::fill_n(std::next(raw_data_set.begin(), 4), 4, _flip(raw_data_set.at(0)));
    for(size_t i = 1; i < 4; ++i)
    {
        raw_data_set.at(i)   = _rotate(raw_data_set.at(0), i*90);
        raw_data_set.at(4+i) = _rotate(raw_data_set.at(4), i*90);
    }

    make_bit();

    //side_length
    for(int i = 0; i < STONE_SIZE; ++i) for(int j = 0; j < STONE_SIZE; ++j)
    {
         if((i == 0 || i == STONE_SIZE -1) && raw_data_set.at(0).at(i).at(j) == 1)
         {
             //std::cout << i << " " << j << " i" << std::endl;
             side_length++;
         }
         if((j == 0 || j == STONE_SIZE -1) && raw_data_set.at(0).at(i).at(j) == 1)
         {
             //std::cout << i << " " << j << " j "<< std::endl;
             side_length++;
         }
         if(j < STONE_SIZE -1 && raw_data_set.at(0).at(i).at(j) != raw_data_set.at(0).at(i).at(j+1))
         {
             //std::cout << i << " " << j << std::endl;
             side_length++;
         }
         if(i < STONE_SIZE -1 && raw_data_set.at(0).at(i).at(j) != raw_data_set.at(0).at(i+1).at(j))
         {
             //std::cout << i << " " << j << std::endl;
             side_length++;
         }
    }

    //area
    area = 0;
    for(auto const& each_raw_data:get_raw_data())
    {
        area += std::count(each_raw_data.begin(),each_raw_data.end(),1);
    }

}

//時計回りを正方向として指定された角度だけ回転する
stone_type::raw_stone_type stone_type::_rotate(raw_stone_type const & raw_data, int angle)
{
    raw_stone_type return_data;
    switch ((angle + 360) / 90 % 4)
    {
        case 0:
            return_data = raw_data;
            break;

        case 1:
            for(int i=0;i<STONE_SIZE;i++) for(int j=0;j<STONE_SIZE;j++)
                return_data[i][j] = raw_data[7-j][i];
            break;

        case 2:
            return_data = raw_data;
            for(auto& each_return_data:return_data)
                std::reverse(each_return_data.begin(),each_return_data.end());
            std::reverse(return_data.begin(),return_data.end());
            break;

        case 3:
            for(int i = 0;i < STONE_SIZE;i++) for(int j = 0;j < STONE_SIZE;j++)
                return_data[i][j] = raw_data[j][7-i];
            break;

        default:
            break;
    }
    return return_data;
}

corner_type stone_type::get_corner()
{
    int a,l,b,r;
    for(a = 0; a < STONE_SIZE && count_n_row(a) == 0; ++a);
    for(l = STONE_SIZE - 1; l >= 0 && count_n_col(l) == 0; --l);
    for(b = STONE_SIZE - 1; b >= 0 && count_n_row(b) == 0; --b);
    for(r = 0; r < STONE_SIZE && count_n_col(r) == 0; ++r);
    std::vector<int> four_corners{
        count_n_row(--a)+count_n_col(--l),//右上
        count_n_col(l)+count_n_row(--b),//右下
        count_n_row(b)+count_n_col(--r),//左下
        count_n_col(r)+count_n_row(a)//左上
    };
    std::vector<int>::iterator iter = std::max_element(four_corners.begin(), four_corners.end());
    std::size_t const best = std::distance(four_corners.begin(), iter);
    switch(best)
    {
    case 0:
        rotate(180);
        return corner_type{four_corners.at(0),point_type{a,r}};
    case 1:
        rotate(90);
        return corner_type{four_corners.at(1),point_type{r,b}};
    case 2:
        return corner_type{four_corners.at(2),point_type{b,l}};
    case 3:
        rotate(270);
        return corner_type{four_corners.at(3),point_type{l,a}};
    default:
        break;
    }
    throw std::runtime_error("error in get_corner"); //ありえない
    return corner_type{0,point_type{0,0}};
}

//#BitSystem
//bitデータの作成
void stone_type::make_bit()
{
    //make bit stones
    for(int x=0;x<41;x++){//マイケルの動き
        for(int flip_c=0;flip_c<2;flip_c++){//flip
            for(int angle=0;angle<4;angle++){//angle
                for(int y=0;y<8;y++){//縦に一行ごと
                    bit_plain_stones[x][flip_c][angle][y]=0;
                    for(int bit_c=0;bit_c<8;bit_c++){
                        bit_plain_stones[x][flip_c][angle][y] += (uint64_t)at(y,bit_c) << (((32+16-1)-x+8)-bit_c);
                    }
                }
                rotate(90);
            }
            flip();
        }
    }
#ifdef _DEBUG
    std::cout<<"bit plain stones"<<std::endl;
    for(int x=0;x<41;x++){//マイケルの動き
        if(x==0 || x==40) std::cout<<"無駄な石"<<std::endl;
        for(int y=0;y<8;y++){//縦に一行ごと
            std::cout<<static_cast<std::bitset<64>>(bit_plain_stones[x][0][0][y])<<std::endl;
        }
        std::cout<<std::endl;
    }
#endif
    for(int i = 5; i < 8; i++)
        raw_data_set.at(i) = _rotate(raw_data_set.at(4), (i - 4) * 90);
}

void stone_type::print_stone() const
{
    for(auto const& each : raw_data_set.at(static_cast<unsigned>(current_side)*4 + current_angle / 90))
    {
        for(auto const& block : each) std::cout << block << " ";
        std::cout << std::endl;
    }

}

void stone_type::_set_random(int const zk)
{
    raw_stone_type candidate;
    do {
        for(auto& row : candidate)
            std::fill(row.begin(), row.end(), 0);

        std::random_device seed_gen;
        std::mt19937_64 engine(seed_gen());
        std::uniform_int_distribution<int> dist_pos(0, STONE_SIZE - 1);
        int x = dist_pos(engine);
        int y = dist_pos(engine);
        candidate.at(y).at(x) = 1;
        for(int count = 1; count < zk; ) {
            x = dist_pos(engine);
            y = dist_pos(engine);
            if (!candidate.at(y).at(x) &&
                ((_is_in_stone(y - 1) && candidate.at(y - 1).at(x)) ||
                 (_is_in_stone(y + 1) && candidate.at(y + 1).at(x)) ||
                 (_is_in_stone(x - 1) && candidate.at(y).at(x - 1)) ||
                 (_is_in_stone(x + 1) && candidate.at(y).at(x + 1)))
                    ) {
                count++;
                candidate.at(y).at(x) = 1;
            }
//            if(insrance++ > FIELD_SIZE * FIELD_SIZE)
//                break;
        }
    } while(_has_hole(candidate));
    _set_from_raw(candidate);
}
void stone_type::_set_from_raw(raw_stone_type raw)
{
    for(int i = 0; i < 4; i++)
        raw_data_set.at(i) = _rotate(raw, i * 90);
    raw_data_set.at(4) = _flip(raw);
    for(int i = 5; i < 8; i++)
        raw_data_set.at(i) = _rotate(raw_data_set.at(4), (i - 4) * 90);
}

/* no new line at end of output */
std::string stone_type::str() const
{
    std::stringstream ss;
    for(auto each_row : get_raw_data()) {
        for(auto const& each_block : each_row) {
            ss << each_block;
        }
        ss << "\r\n";
    }
    return std::move(ss.str());
}


/* returns if stone has any hole */
bool stone_type::_has_hole(raw_stone_type stone)
{
    auto stone_ = stone;

    // stoneの(y, x)を見て，周り全てがstone_.at(i).at(j)==1に達すれば穴
    // 隣に拡散して再帰する
    std::function<bool(raw_stone_type&, std::size_t const, std::size_t const)> is_hole;
    is_hole = [&is_hole, this](raw_stone_type& stone, std::size_t const y, std::size_t const x) -> bool
    {
        // 指定されたzkの確認
        if (stone.at(y).at(x)) return true; // 探索済み or zkである
        else stone.at(y).at(x) = 2;         // 探索済みフラグ

        // 1つでも端に達すれば穴ではないが，そうでなければ再帰して確認
        bool result = true;
        result = result && ((!_is_in_stone(y + 1, x)) ? false : is_hole(stone, y + 1, x));
        result = result && ((!_is_in_stone(y - 1, x)) ? false : is_hole(stone, y - 1, x));
        result = result && ((!_is_in_stone(y, x + 1)) ? false : is_hole(stone, y, x + 1));
        result = result && ((!_is_in_stone(y, x - 1)) ? false : is_hole(stone, y, x - 1));

        return result;
    };

    bool result = false;
    for (std::size_t i = 0; i < stone_.size(); i++)
    {
        for (std::size_t j = 0; j < stone_.at(i).size(); j++)
        {
            if (!stone_.at(i).at(j)) // まだ見てないzkかどうか
            {
                result = result || is_hole(stone_, i, j);
            }
        }
    }

    return result;
}
