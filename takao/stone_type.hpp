#ifndef STONE_TYPE
#define STONE_TYPE

// 石
class SHARED_EXPORT stone_type
{
    public:
        enum struct Sides {Head = 0, Tail = 1};
        typedef std::array<std::array<int,8>,8> raw_stone_type;

        stone_type() = default;
        ~stone_type() = default;

        stone_type(std::string const & raw_stone_text, int const _nth);

        friend bool operator== (stone_type const& lhs, stone_type const& rhs);

        int & at(size_t y,size_t x);
        int const & at(size_t y,size_t x) const;
        raw_stone_type const & get_raw_data() const;
        stone_type& rotate(int angle);
        stone_type& flip();
        size_t get_area() const;
        stone_type::Sides get_side() const;
        std::size_t get_angle() const;

    private:
        raw_stone_type raw_data;
        int nth;
        std::array<raw_stone_type,8>  raw_data_set;
        Sides current_side = Sides::Head;
        std::size_t current_angle = 0;

        raw_stone_type _rotate(int angle);
        raw_stone_type _flip(raw_stone_type stone);
};

bool operator== (stone_type const& lhs, stone_type const& rhs)
{
    return lhs.raw_data == rhs.raw_data;
}

stone_type::stone_type(std::string const & raw_stone_text, int const _nth) :nth(_nth)
{
    auto rows = _split(raw_stone_text, "\r\n");
    for (std::size_t i = 0; i < raw_data.size(); ++i) {
        std::transform(rows[i].begin(), rows[i].end(), raw_data[i].begin(),
                       [](auto const & c) { return c == '1'; });
    }

    //rotate用のarrayを準備する
    raw_data_set.at(0)  =  raw_data;
    raw_data_set.at(4) = std::move(_flip(raw_data));
    for(size_t i = 1; i < 4; ++i)
    {
        raw_data_set.at(i) = std::move(_rotate(i*90));
        raw_data_set.at(4+i) = std::move(_flip(raw_data_set.at(i)));
    }
}

//生配列へのアクセサ
//座標を受け取ってそこの値を返す
int const & stone_type::at(size_t y,size_t x) const
{
    return raw_data.at(y).at(x);
}

int & stone_type::at(size_t y,size_t x)
{
    return const_cast<int &>(at(y, x));
}

//石へのアクセサ
//生配列への参照を返す
stone_type::raw_stone_type const& stone_type::get_raw_data() const
{
    return raw_data_set.at(static_cast<unsigned>(current_side) + current_angle / 90);
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
    for(auto const& each_raw_data:raw_data)
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
stone_type::raw_stone_type stone_type::_rotate(int angle)
{
    raw_stone_type return_data;

    switch ((angle + 360)/90)
    {
        case 0:
            return_data = raw_data;
            break;

        case 1:
            for(int i=0;i<8;i++) for(int j=0;j<8;j++)
            {
                return_data[i][j] = raw_data[j-7][i];
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
            for(int i = 0;i < 8;i++) for(int j = 0;j < 8;j++)
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

#endif // STONE_TYPE
