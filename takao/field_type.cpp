
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "field_type.hpp"
#include "utils.hpp"
//石が置かれているか否かを返す 置かれているときtrue 置かれていないときfalse
bool field_type::is_placed(stone_type const& stone)
{
    return std::find_if(processes.begin(), processes.end(),
                        [& stone](auto const & process) { return process.stone == stone; }
           ) != processes.end();
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
    for(int i = 0; i < STONE_SIZE; ++i) for(int j = 0; j < STONE_SIZE; ++j)
    {
        if(y+i < 0 || x+j < 0 || y+i > 31 || x+j > 31) continue;
        else if(stone.at(i,j) == 1)
        {
            raw_data.at(i+y).at(j+x) = stone.get_nth();
        }
    }
    processes.emplace_back(stone, point_type{y, x});
    return *this;
}

//指定された場所に指定された石が置けるかどうかを返す
bool field_type::is_puttable(stone_type const& stone, int y, int x)
{
    bool is_connection = false;
    if(processes.size() == 0)
    {
        is_connection = true;//始めの石なら繋がりは必要ない
        //std::cout << "first stone" << std::endl;
    }
    for(int i = 0; i < STONE_SIZE; ++i) for(int j = 0; j < STONE_SIZE; ++j)
    {
        if(stone.at(i,j) == 0)//置かないならどうでも良い
        {
            continue;
        }
        else if(raw_data.at(y+i).at(j+x) != 0)//石または障害物の上へ石を置こうとした
        {
            //std::cout << "You try to put the stone on another stone." << std::endl;
            return false;
        }
        else if(y+i < 0 || x+j < 0 || y+i > 31 || x+j > 31)//敷地外に石を置こうとした
        {
            //std::cout << "You try to put the stone out of range" << std::endl;
            return false;
        }
        if(is_connection == true) continue;
        else
        {
            if(i+y > 0  && raw_data.at(i+y-1).at(j+x) > 0 && raw_data.at(i+y-1).at(j+x) < stone.get_nth()) is_connection = true;
            if(i+y < 31 && raw_data.at(i+y+1).at(j+x) > 0 && raw_data.at(i+y+1).at(j+x) < stone.get_nth()) is_connection = true;
            if(j+x < 0  && raw_data.at(i+y).at(j+x-1) > 0 && raw_data.at(i+y).at(j+x-1) < stone.get_nth()) is_connection = true;
            if(j+x < 31 && raw_data.at(i+y).at(j+x+1) > 0 && raw_data.at(i+y).at(j+x+1) < stone.get_nth()) is_connection = true;
        }
    }
    //if(is_connection == false) std::cout << "This stone cannot put here becase there is not connection." << std::endl;
    return is_connection;
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
    processes.erase(std::remove_if(processes.begin(), processes.end(),
                                   [& stone](auto const & process) { return process.stone == stone; }),
                    processes.end());
    return *this;
 }

//指定された石を取り除けるかどうかを返す
bool field_type::is_removable(stone_type const& stone)
 {
     std::vector<pair_type> pair_list;
     std::vector<pair_type> remove_list;
     if(is_placed(stone) == false) return false;
     if(processes.size() == 1)return true;
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
 std::vector<stone_type> field_type::list_of_stones() const
 {
     std::vector<stone_type> result;
     std::transform(processes.begin(), processes.end(), std::back_inserter(result),
                    [](auto const & process) { return process.stone; });
     return result;
 }

//コメント書こう
 placed_stone_type field_type::get_stone(std::size_t const & y, std::size_t const & x)
{
    auto nth = raw_data.at(y).at(x);
    if (nth == 0 || nth == -1) {
        throw std::runtime_error("There is no stone.");
    }

    point_type pf = std::find_if(processes.begin(), processes.end(),
                                 [& nth](auto const & process) { return process.stone.get_nth() == nth; }
                                )->position;
    point_type ps = {static_cast<int>(y) - pf.y, static_cast<int>(x) - pf.x};
    const stone_type * stone;

    for (auto & process : processes) {
        if (process.stone.get_nth() == nth) {
            stone = &process.stone;
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

std::string field_type::get_answer() const
{
    std::string result;
    int prev_nth = 0;
    int process_count=0;
    for (auto const & process : processes) {
        std::string line;

        auto current_nth = process.stone.get_nth();

        // スキップ分の改行を挿入する
        for (int i = prev_nth + 1; i < current_nth; ++i) {
            result.append("\r\n");
        }
        if(process_count!=0) result.append("\r\n");
        line += std::to_string(process.position.x)
                + " "
                + std::to_string(process.position.y)
                + " "
                + (process.stone.get_side() == stone_type::Sides::Head ? "H" : "T")
                + " "
                + std::to_string(process.stone.get_angle());
        result.append(line);
        prev_nth = current_nth;
        process_count++;
    }
    return result;
}

