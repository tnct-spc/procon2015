#ifndef FIELD_TYPE
#define FIELD_TYPE
#include <iostream>
// 敷地
class SHARED_EXPORT field_type
{
    public:
        typedef std::array<std::array<int, 32>, 32> raw_field_type;

        field_type() = default;
        ~field_type() = default;

        field_type(std::string const & raw_field_text);

        //現在の状態における得点を返す
        size_t get_score();

        //石を置く  自身への参照を返す   失敗したら例外を出す
        field_type& put_stone(stone_type const& stone, int y, int x);

        //指定された場所に指定された石が置けるかどうかを返す
        bool is_puttable(stone_type const& stone, int y, int x);


        //指定された石を取り除く．その石が置かれていない場合, 取り除いた場合に不整合が生じる場合は例外を出す
        field_type& remove_stone(stone_type const& stone);

        //指定された石を取り除けるかどうかを返す
        bool is_removable(stone_type const& stone);

        //置かれた石の一覧を表す配列を返す
        std::vector<stone_type> const & list_of_stones() const;

        //
        placed_stone_type get_stone(std::size_t const & y, std::size_t const & x);

        raw_field_type const & get_raw_data() const;

    private:
        raw_field_type raw_data;
        std::array<std::array<int, 32>, 32> placed_order;
        std::vector<stone_type> placed_stone_list;
        std::array<point_type, 257> reference_point;

        //is_removableで必要
        struct pair_type
        {
            int a;
            int b;
        };

        //石が置かれているか否かを返す
        bool is_placed(stone_type const& stone);
};

//石が置かれているか否かを返す
bool field_type::is_placed(stone_type const& stone)
{
    for(auto const& each_placed_stone : placed_stone_list)
    {
        if(each_placed_stone == stone) return true;
    }
    return false;
}

//現在の状態における得点を返す
size_t field_type::get_score()
{
    size_t sum = 0;
    for (auto const & row : raw_data)
    {
        sum += std::count(row.begin(), row.end(), 0);
    }
    return sum;
}

//石を置く  自身への参照を返す   失敗したら例外を出す
field_type& field_type::put_stone(stone_type const& stone, int y, int x)
{
    //さきに置けるか確かめる
    for(int i = 0; i < 8; ++i) for(int j = 0; j < 8; ++j)
    {
        if(stone.at(i,j) == 0)//置かないならどうでも良い
        {
            continue;
        }
        else if(i+y < 0 || j+x < 0)//敷地外に石を置こうとした
        {
            throw std::runtime_error("Puted the stone out of rang.");
         }
        else if(raw_data.at(i+y).at(x+j) == 0)//敷地が0ならいいよ！
        {
            continue;
        }
        else if(raw_data.at(i+y).at(j+x) != 0 && stone.at(i,j) == 1)
        {
            throw std::runtime_error("Failed to put the stone.");
        }
    }
    //置く
    for(int i = 0; i < 8; ++i) for(int j = 0; j < 8; ++j)
    {
        if(i+y < 0 || j+x < 0) continue;
        else if(stone.at(i,j) == 1)
        {
            raw_data.at(i+y).at(j+x) = stone.get_nth();
            placed_order.at(i+y).at(j+x) = stone.get_nth();
            placed_stone_list.push_back(stone);
        }
    }

    reference_point.at(stone.get_nth()) = point_type{y,x};
    return *this;
}

//指定された場所に指定された石が置けるかどうかを返す
bool field_type::is_puttable(stone_type const& stone, int y, int x)
{
    for(size_t i = 0;i < 8;++i)
    {
        for(size_t j = 0;j < 8;++j)
        {
            if(raw_data.at(i+y).at(j+x) == 0) continue;
            else if(stone.at(i,j) == 0)continue;
            else return false;
        }
    }
    return true;
}

//指定された石を取り除く．その石が置かれていない場合, 取り除いた場合に不整合が生じる場合は例外を出す
field_type& field_type::remove_stone(stone_type const& stone)
{
    if (is_placed(stone) == false)
    {
        throw std::runtime_error("The stone isn't placed.");
    }
    else if(is_removable(stone) == false)
    {
        throw std::runtime_error("The stone can't remove.");
    }
    for(auto const& each_placed_order : placed_order) for(int each_block:each_placed_order)
    {
        if(each_block == stone.get_nth()) each_block = 0;
    }
    return *this;
 }

//指定された石を取り除けるかどうかを返す
bool field_type::is_removable(stone_type const& stone)
 {
     std::vector<pair_type> pair_list;
     std::vector<pair_type> remove_list;

     if(is_placed(stone) == false) throw std::runtime_error("The stone isn't' placed");

     //継ぎ目を検出
     for(size_t i = 0; i < 39; ++i) for(size_t j = 0; j < 39; ++j)
     {
         int const c = placed_order.at(i).at(j);
         int const d = placed_order.at(i+1).at(j);
         int const r = placed_order.at(i).at(j+1);
         if(c != d) pair_list.push_back(pair_type{c,d});
         if(c != r) pair_list.push_back(pair_type{c,r});
     }
     //取り除きたい石に隣接している石リストを作りながら、取り除きたい石を含む要素を消す
     for(std::vector<pair_type>::iterator it = pair_list.begin();it != pair_list.end();)
     {
         if(it->a == stone.get_nth() || it->b == stone.get_nth())
         {
             remove_list.push_back(*it);
             it = pair_list.erase(it);
         }
         else ++it;
     }
     //取り除きたい石に隣接している石リストに含まれる石それぞれに、不整合が生じていないか見ていく
     bool ans = false;
     for(auto const& each_remove_list : remove_list)
     {
         int const target_stone_num = (each_remove_list.a == stone.get_nth())?each_remove_list.b:each_remove_list.a;
         for(auto const& each_pea_list : pair_list)
         {
             if((each_pea_list.a == target_stone_num && each_pea_list.a > each_pea_list.b) ||
                (each_pea_list.b == target_stone_num && each_pea_list.b > each_pea_list.a))
             {
                 ans = true;
                 break;
             }
             else continue;
         }
         if(ans == false) return false;
         else continue;
     }
     return true;
 }

 //置かれた石の一覧を表す配列を返す
 std::vector<stone_type> const & field_type::list_of_stones() const
 {
     return placed_stone_list;
 }

//コメント書こう
 placed_stone_type field_type::get_stone(std::size_t const & y, std::size_t const & x)
{
    auto nth = raw_data.at(y).at(x);
    if (nth == 0 || nth == -1) {
        throw std::runtime_error("There is no stone.");
    }

    point_type pf = reference_point[nth];
    point_type ps = {pf.y - static_cast<int>(y), pf.x - static_cast<int>(x)};
    stone_type * stone;

    for (auto & placed_stone : placed_stone_list) {
        if (placed_stone.get_nth() == nth) {
            stone = &placed_stone;
            break;
        }
    }

    return placed_stone_type(*stone, pf, ps);
}

 //コメント書こう
field_type::field_type(std::string const & raw_field_text)
{
    auto rows = _split(raw_field_text, "\r\n");
    for (std::size_t i = 0; i < raw_data.size(); ++i) {
        std::transform(rows[i].begin(), rows[i].end(), raw_data[i].begin(),
                       [](auto const & c) { return c == '1' ? -1 : 0; });
    }
}

field_type::raw_field_type const & field_type::get_raw_data() const
{
    return raw_data;
}

#endif // FIELD_TYPE
