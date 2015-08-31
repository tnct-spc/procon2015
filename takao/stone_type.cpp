//#define _DEBUG

#ifdef _DEBUG
#include <QDebug>
#include <iostream>
#endif

#include "stone_type.hpp"
#include "utils.hpp"
#include "point_type.hpp"
#include <stdexcept>
#include <algorithm>
#include <array>
#include <bitset>

// 石

bool operator== (stone_type const& lhs, stone_type const& rhs)
{
    return lhs.get_raw_data() == rhs.get_raw_data();
}

stone_type::stone_type(std::string const & raw_stone_text, int const _nth) :nth(_nth)
{
    auto rows = _split(raw_stone_text, "\r\n");
    for (std::size_t i = 0; i < raw_data_set.at(0).size(); ++i) {
        std::transform(rows[i].begin(), rows[i].end(), raw_data_set.at(0)[i].begin(),
                       [](auto const & c) { return c == '1'; });
    }

    //rotate用のarrayを準備する
    std::fill_n(std::next(raw_data_set.begin(), 4), 4, _flip(raw_data_set.at(0)));
    for(size_t i = 1; i < 4; ++i)
    {
        raw_data_set.at(i)   = _rotate(raw_data_set.at(0), i*90);
        raw_data_set.at(4+i) = _rotate(raw_data_set.at(4), i*90);
    }

    make_bit();
}

//生配列へのアクセサ
//座標を受け取ってそこの値を返す
int const & stone_type::at(size_t y,size_t x) const
{
    return get_raw_data().at(y).at(x);
}

int stone_type::at(size_t y,size_t x)
{
    return get_raw_data().at(y).at(x);
}

//石へのアクセサ
//生配列への参照を返す
stone_type::raw_stone_type const& stone_type::get_raw_data() const
{
    return raw_data_set.at(static_cast<unsigned>(current_side)*4 + current_angle / 90);
}

//時計回りを正方向として指定された角度だけ回転する
// 自身への参照を返す
stone_type& stone_type::rotate(int angle)
{
    current_angle = (current_angle + angle) % 360;
    return *this;
}

//左右に反転する
//自身への参照を返す
stone_type& stone_type::flip()
{
    current_side = current_side == Sides::Head ? Sides::Tail : Sides::Head;
    return *this;
}

//面積を返す
size_t stone_type::get_area() const
{
    size_t sum = 0;
    for(auto const& each_raw_data:get_raw_data())
    {
        sum += std::count(each_raw_data.begin(),each_raw_data.end(),1);
    }
    return sum;
}

//現在の表裏を返す
stone_type::Sides stone_type::get_side() const
{
    return current_side;
}

//現在の角度を返す
std::size_t stone_type::get_angle() const
{
    return current_angle;
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
            {
                return_data[i][j] = raw_data[7-j][i];
            }
            break;

        case 2:
            return_data = raw_data;
            for(auto& each_return_data:return_data)
            {
                std::reverse(each_return_data.begin(),each_return_data.end());
            }
            std::reverse(return_data.begin(),return_data.end());
            break;

        case 3:
            for(int i = 0;i < STONE_SIZE;i++) for(int j = 0;j < STONE_SIZE;j++)
            {
                return_data[i][j] = raw_data[j][7-i];
            }
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

//n列目のブロック数を返す
int stone_type::count_n_row(int const n)const
{
    int const current_data = static_cast<unsigned>(current_side)*4 + current_angle / 90;
    return std::count(raw_data_set.at(current_data).at(n).begin(),raw_data_set.at(current_data).at(n).end(),1);
}

//n行目のブロック数を返す
int stone_type::count_n_col(int const n)const
{
    int const current_data = static_cast<unsigned>(current_side)*4 + current_angle / 90;
    int sum = 0;
    for(int i = 0;i < STONE_SIZE; ++i) sum += raw_data_set.at(current_data).at(i).at(n);
    return sum;
}


//左右に反転する
stone_type::raw_stone_type stone_type::_flip(raw_stone_type stone)
{
    for(auto& each_stone:stone)
    {
        std::reverse(each_stone.begin(),each_stone.end());
    }
    return stone;
}

int stone_type::get_nth()const
{
    return nth;
}

int stone_type::get_side_length()const
{
    int sum = 0;
    for(int i = 0; i < STONE_SIZE -1; ++i) for(int j = 0; j < STONE_SIZE - 1; ++j)
    {
         if(raw_data_set.at(0).at(i).at(j) != raw_data_set.at(0).at(i).at(j+1))sum++;
         if(raw_data_set.at(0).at(i).at(j) != raw_data_set.at(0).at(i+1).at(j))sum++;
    }
    return sum;
}

//#BitSystem
//bitデータの作成
void stone_type::make_bit()
{
    auto is_side = [&](int &y, int &x) -> bool
    {
        if(1<=x && x<=8){
            if(y>=2) if(at(y-1-1,x-1)) return true;
            if(y<=7) if(at(y-1+1,x-1)) return true;
        }
        if(1<=y && y<=8){
            if(x>=2) if(at(y-1,x-1-1)) return true;
            if(x<=7) if(at(y-1,x-1+1)) return true;
        }
        return false;
    };

    //make bit stones
    for(int x=0;x<39;x++){//マイケルの動き
        for(int flip_c=0;flip_c<2;flip_c++){//flip
            for(int angle=0;angle<4;angle++){//angle
                for(int y=0;y<8;y++){//縦に一行ごと
                    bit_plain_stones[x][flip_c][angle][y]=0;
                    for(int bit_c=0;bit_c<8;bit_c++){
                        bit_plain_stones[x][flip_c][angle][y] += (uint64_t)at(y,bit_c) << ((64-10-x)-bit_c);
                    }
                }
                for(int y=0;y<10;y++){//縦に一行ごと
                    bit_side_stones[x][flip_c][angle][y]=0;
                    for(int bit_c=0;bit_c<10;bit_c++){
                        bit_side_stones[x][flip_c][angle][y] += (uint64_t)is_side(y,bit_c) << ((64-10-x)-bit_c);
                    }
                }
                rotate(90);
            }
            flip();
        }
    }

#ifdef _DEBUG
    std::cout<<"bit plain stones"<<std::endl;
    //for(int x=0;x<39;x++){//マイケルの動き
        for(int y=0;y<8;y++){//縦に一行ごと
            std::cout<<static_cast<std::bitset<64>>(bit_plain_stones[0][0][0][y])<<std::endl;
        }
        std::cout<<std::endl;
    //}
    std::cout<<"bit side stones"<<std::endl;
    //for(int x=0;x<39;x++){//マイケルの動き
        for(int y=0;y<10;y++){//縦に一行ごと
            std::cout<<static_cast<std::bitset<64>>(bit_side_stones[0][0][0][y])<<std::endl;
        }
        std::cout<<std::endl;
    //}
#endif
}
