#include "stone_type.hpp"
#include "utils.hpp"
#include <algorithm>
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
}

//生配列へのアクセサ
//座標を受け取ってそこの値を返す
int const & stone_type::at(size_t y,size_t x) const
{
    return get_raw_data().at(y).at(x);
}

int & stone_type::at(size_t y,size_t x)
{
    return const_cast<int &>(at(y, x));
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
