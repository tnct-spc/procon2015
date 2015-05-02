#ifndef FIELD_TYPE
#define FIELD_TYPE
#include <iostream>
#include <iomanip>
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
        std::vector<stone_type> placed_stone_list;
        std::array<point_type, 257> reference_point;
        point_type static constexpr not_puted = {32,32};

        //is_removableで必要
        struct pair_type
        {
            int a;
            int b;
        };

        //石が置かれているか否かを返す
        bool is_placed(stone_type const& stone);

        void print_field();
};

//石が置かれているか否かを返す 置かれているときtrue 置かれていないときfalse
bool field_type::is_placed(stone_type const& stone)
{
    //std::cout << "reference_point.at("<< stone.get_nth() <<") = {" <<  reference_point.at(stone.get_nth()).x << "," << reference_point.at(stone.get_nth()).y <<"}" << std::endl;
    return reference_point.at(stone.get_nth()) == point_type{32,32} ? false : true;
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
    if(is_puttable(stone,y,x) == false)throw std::runtime_error("The stone cannot put.");
    //置く
    for(int i = 0; i < 8; ++i) for(int j = 0; j < 8; ++j)
    {
        if(i+y < 0 || j+x < 0) continue;
        else if(stone.at(i,j) == 1)
        {
            raw_data.at(i+y).at(j+x) = stone.get_nth();
        }
    }
    placed_stone_list.push_back(stone);
    reference_point.at(stone.get_nth()) = point_type{y,x};
    return *this;
}

//指定された場所に指定された石が置けるかどうかを返す
bool field_type::is_puttable(stone_type const& stone, int y, int x)
{
    bool is_conection = false;
    for(int i = 0; i < 8; ++i) for(int j = 0; j < 8; ++j)
    {
        if(stone.at(i,j) == 0)//置かないならどうでも良い
        {
            continue;
        }
        else if(i+y < 0 || j+x < 0)//敷地外に石を置こうとした
        {
            return false;
         }
        else if(raw_data.at(i+y).at(j+x) == 0)//敷地が0ならいいよ！
        {
            if(is_conection == true) continue;
            else if(i+y > 1   && raw_data.at(i+y-1).at(j+x   ) < stone.get_nth()) is_conection = true;
            else if(i+y < 30 && raw_data.at(i+y+1).at(j+x  ) < stone.get_nth()) is_conection = true;
            else if(j+x > 1   && raw_data.at(i+y    ).at(j+x-1) < stone.get_nth()) is_conection = true;
            else if(j+x < 30 && raw_data.at(i+y    ).at(j+x+1) < stone.get_nth()) is_conection = true;
            continue;
        }
        else if(raw_data.at(y+i).at(j+x) != 0 && stone.at(i,j) == 1)
        {
            return false;
        }
    }
    return true;
}

//指定された石を取り除く．その石が置かれていない場合, 取り除いた場合に不整合が生じる場合は例外を出す
field_type& field_type::remove_stone(stone_type const& stone)
{
    if (is_placed(stone) == false)
    {
        throw std::runtime_error("The stone isn't placed...in remove_stone");
    }
    else if(is_removable(stone) == false)
    {
        throw std::runtime_error("The stone can't remove.");
    }
    for(int i = 0; i < 32; ++i) for(int j = 0; j < 32; ++j)
    {
        if(raw_data.at(i).at(j) == stone.get_nth())raw_data.at(i).at(j) = 0;
    }
    reference_point.at(stone.get_nth()) = point_type{32,32};
    auto result = std::remove_if(placed_stone_list.begin(), placed_stone_list.end(),[stone](stone_type const& list_stone) { return list_stone == stone; });
    placed_stone_list.erase(result, placed_stone_list.end());
    return *this;
 }

//指定された石を取り除けるかどうかを返す
bool field_type::is_removable(stone_type const& stone)
 {
     std::vector<pair_type> pair_list;
     std::vector<pair_type> remove_list;
     if(is_placed(stone) == false) return false;
     if(placed_stone_list.size() == 1)return true;
     //継ぎ目を検出
     for(size_t i = 0; i < 31; ++i) for(size_t j = 0; j < 31; ++j)
     {
         int const c = raw_data.at(i).at(j);
         int const d = raw_data.at(i+1).at(j);
         int const r = raw_data.at(i).at(j+1);
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
             if((each_pea_list.a == target_stone_num && each_pea_list.a > each_pea_list.b && each_pea_list.b > 0) ||
                (each_pea_list.b == target_stone_num && each_pea_list.b > each_pea_list.a && each_pea_list.a > 0))
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
    std::fill(reference_point.begin(),reference_point.end(),point_type{32,32});
}

field_type::raw_field_type const & field_type::get_raw_data() const
{
    return raw_data;
}

void field_type::print_field()
{
    for(auto const&each_raw : raw_data)
    {
        for(auto each_block : each_raw)
        {
            std::cout << std::setw(3) << each_block;
        }
        std::cout << std::endl;
    }
}

#endif // FIELD_TYPE
