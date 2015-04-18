#ifndef TAKAO_HPP
#define TAKAO_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <string>
#include <tuple>
#include <vector>
#include "takao_global.hpp"
#include <array>
#include <cstdint>
#include <algorithm>

typedef std::array<std::array<int,8>,8> raw_stone_type;

using namespace std::string_literals;

// SHARED_EXPORT って書けば外から見える

// 文字列を文字列のデリミタにより分割する
std::vector<std::string> _split(std::string const & target, std::string const & delimiter)
{
    std::size_t begin = 0, end;
    auto const delimiter_length = delimiter.size();
    std::vector<std::string> result;

    while (begin <= target.size()) {
        end = target.find(delimiter, begin);
        if (end == std::string::npos) {
            end = target.size();
        }
        result.push_back(target.substr(begin, end - begin));
        begin = end + delimiter_length;
    }

    return result;
}


// 敷地のブロックの状態
enum struct BlockState { BLANK, STONE, OBSTACLE };


// 何らかの座標を表すクラス
class SHARED_EXPORT point_type
{
    public:
        point_type() = default;
        ~point_type() = default;

        int y;
        int x;
};

// 石
class SHARED_EXPORT stone_type
{
    public:
        enum Sides {Head, Tail};

    private:
        raw_stone_type raw_data;
        int nth;
        std::array<raw_stone_type,8>  raw_data_set;
        Sides current_side;
        std::size_t current_angle;

        //時計回りを正方向として指定された角度だけ回転する
        raw_stone_type _rotate(int angle)
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
        raw_stone_type _flip(raw_stone_type stone)
        {
            for(auto& each_stone:stone)
            {
                std::reverse(each_stone.begin(),each_stone.end());
            }
            return stone;
        }

    public:
        stone_type() = default;
        ~stone_type() = default;

        friend inline bool operator== (stone_type const& lhs, stone_type const& rhs)
        {
            return lhs.raw_data == rhs.raw_data;
        }

        stone_type(std::string const & raw_stone_text);

        //生配列へのアクセサ
        //座標を受け取ってそこの値を返す
        int const & at(size_t y,size_t x) const
        {
            return raw_data.at(y).at(x);
        }

        int & at(size_t y,size_t x)
        {
            return const_cast<int &>(at(y, x));
        }

        //石へのアクセサ
        //生配列への参照を返す
        raw_stone_type const& get_array()
        {
            return raw_data_set.at(current_side + current_angle * 4);
        }

        //時計回りを正方向として指定された角度だけ回転する
        // 自身への参照を返す
        stone_type& rotate(int angle)
        {
            current_angle = (current_angle + angle) % 360;
            return *this;
        }

        //左右に反転する
        //自身への参照を返す
        stone_type& flip()
        {
            current_side = current_side == Sides::Head ? Sides::Tail : Sides::Head;
            return *this;
        }

        //面積を返す
        size_t get_area()
        {
            size_t sum = 0;
            for(auto const& each_raw_data:raw_data)
            {
                sum += std::count(each_raw_data.begin(),each_raw_data.end(),1);
            }
            return sum;
        }

        stone_type::Sides get_side() const;
        std::size_t get_angle() const;
};

stone_type::stone_type(std::string const & raw_stone_text)
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

stone_type::Sides stone_type::get_side() const
{
    return current_side;
}

std::size_t stone_type::get_angle() const
{
    return current_angle;
}

// 敷地に置かれた石の情報
class SHARED_EXPORT placed_stone_type
{
    public:
        placed_stone_type() = default;
        ~placed_stone_type() = default;

        placed_stone_type(stone_type const & _stone,
                          point_type const & pf,
                          point_type const & ps)
        : stone(_stone), p_in_field(pf), p_in_stone(ps) {}

        stone_type const & stone;
        point_type p_in_field;
        point_type p_in_stone;
};

// 敷地
class SHARED_EXPORT field_type
{
    private:
        std::array<std::array<int,40>,40> raw_data;
        std::array<std::array<int,32>,32> placed_order;
        std::vector<stone_type> placed_stone_list;
        std::array<point_type,257> reference_point;

        //is_removableで必要
        struct pea_type
        {
            int a;
            int b;
        };

        //石が置かれているか否かを返す
        bool is_placed(stone_type const& stone)
        {
            for(auto const& each_placed_stone : placed_stone_list)
            {
                if(each_placed_stone == stone) return true;
            }
            return false;
        }

    public:
        field_type() = default;
        ~field_type() = default;

        field_type(std::string const & raw_field_text);

        //現在の状態における得点を返す
        size_t get_score()
        {
            size_t sum = 0;
            for(size_t i = 8; i < 40; ++i)
            {
                sum += std::count(raw_data.at(i).begin()+8,raw_data.at(i).end(),0);
            }
            return sum;
        }

        //石を置く  自身への参照を返す   失敗したら例外を出す
        field_type& put_stone(stone_type const& stone, int y, int x)
        {
            //さきに置けるか確かめる
            for(int i = 0; i < 8; ++i) for(int j = 0; j < 8; ++j)
            {
                if(raw_data.at(i+y+8).at(j+x+8) == 1 && stone.at(i,j) == 1) throw std::runtime_error("Failed to put the stone.");
            }
            //置く
            for(int i = 0; i < 8; ++i) for(int j = 0; j < 8; ++j)
            {
                if(raw_data.at(i+y+8).at(j+x+8) == 0 && stone.at(i,j) == 1)
                {
                    raw_data.at(i+y+8).at(j+x+8) = stone.at(i,j);
                    placed_order.at(i+y).at(j+x) = stone.nth; //TODO:nthコンストラクタで代入してこれできるようにする
                    placed_stone_list.push_back(stone);
                }
            }
            reference_point.at(stone.nth) = point_type{y,x};
            return *this;
        }

        //指定された場所に指定された石が置けるかどうかを返す
        bool is_puttable(stone_type const& stone, int y, int x)
        {
            for(size_t i = 0;i < 8;++i)
            {
                for(size_t j = 0;j < 8;++j)
                {
                    if(raw_data.at(i+y+8).at(j+x+8) == 0) continue;
                    else if(stone.at(i,j) == 0)continue;
                    else return false;
                }
            }
            return true;
        }

        //指定された石を取り除く
        //その石が置かれていない場合, 取り除いた場合に不整合が生じる場合は例外を出す
        field_type& remove_stone(stone_type const& stone)
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
                if(each_block == stone.nth) each_block = 0;
            }
            return *this;
         }

         //指定された石を取り除けるかどうかを返す
        bool is_removable(stone_type const& stone)
        {
            std::vector<pea_type> pea_list;
            std::vector<pea_type> remove_list;

            //NOTE:仕様にはないからコメントアウトしとくけど、あった方が良いと思う
            //if(is_placed(stone) == false) throw std::runtime_error("The stone isn't' placed");

            //継ぎ目を検出
            for(size_t i = 0; i < 39; ++i) for(size_t j = 0; j < 39; ++j)
            {
                int const c = placed_order.at(i).at(j);
                int const d = placed_order.at(i+1).at(j);
                int const r = placed_order.at(i).at(j+1);
                if(c != d) pea_list.push_back(pea_type{c,d});
                if(c != r) pea_list.push_back(pea_type{c,r});
            }
            //取り除きたい石に隣接している石リストを作りながら、取り除きたい石を含む要素を消す
            for(std::vector<pea_type>::iterator it = pea_list.begin();it != pea_list.end();)
            {
                if(it->a == stone.nth || it->b == stone.nth)
                {
                    remove_list.push_back(*it);
                    it = pea_list.erase(it);
                }
                else ++it;
            }
            //取り除きたい石に隣接している石リストに含まれる石それぞれに、不整合が生じていないか見ていく
            bool ans = false;
            for(auto const& each_remove_list : remove_list)
            {
                int const target_stone_num = (each_remove_list.a == stone.nth)?each_remove_list.b:each_remove_list.a;
                for(auto const& each_pea_list : pea_list)
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
        std::vector<stone_type> const & list_of_stones() const
        {
            return placed_stone_list;
        }

        placed_stone_type get_stone(std::size_t const & y, std::size_t const & x);
};

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
        if (placed_stone.nth == nth) {
            stone = &placed_stone;
            break;
        }
    }

    return placed_stone_type(*stone, pf, ps);
}

field_type::field_type(std::string const & raw_field_text)
{
    auto rows = _split(raw_field_text, "\r\n");
    for (std::size_t i = 0; i < raw_data.size(); ++i) {
        std::transform(rows[i].begin(), rows[i].end(), raw_data[i].begin(),
                       [](auto const & c) { return c == '1'; });
    }
}

//-----------------------------------------------------------
// 問題データ
class SHARED_EXPORT problem_type
{
    public:
        problem_type() = default;
        ~problem_type() = default;

        problem_type(std::string const & problem_text);

        field_type field;
        std::vector<stone_type> stones;

    private:
        static std::tuple<std::string, std::vector<std::string>> _split_problem_text(std::string const & problem_text);
};

problem_type::problem_type(std::string const & problem_text)
{
    auto && split = _split_problem_text(problem_text);
    auto & field_text = std::get<0>(split);
    auto & stone_texts = std::get<1>(split);

    field = field_type(field_text);
    for (auto const & stone_text : stone_texts) {
        stones.emplace_back(stone_text);
    }
}

std::tuple<std::string, std::vector<std::string>> problem_type::_split_problem_text(std::string const & problem_text)
{
    std::tuple<std::string, std::vector<std::string>> result;
    auto & field = std::get<0>(result);
    auto & stones = std::get<1>(result);

    auto split = _split(problem_text, "\r\n\r\n"s);

    field = split.front();

    std::copy(std::next(split.begin()),
              split.end(),
              std::back_inserter(stones));
    stones.front().erase(0, stones.front().find('\n') + 1);

    return result;
}

// 解答データの手順ひとつ分
class SHARED_EXPORT process_type
{
    public:
        process_type() = default;
        ~process_type() = default;

        process_type(stone_type const & _stone,
                     point_type const & _position)
        : stone(_stone), position(_position) {}

        stone_type const & stone;
        point_type position;
};

// 解答データ
class SHARED_EXPORT answer_type
{
    public:
        answer_type() = default;
        ~answer_type() = default;

        answer_type(problem_type const & _problem) : problem(_problem) {}

        std::string get_answer_str() const;

        std::vector<process_type> processes;

    private:
        problem_type const & problem;
};

// 解答文字列を返す
std::string answer_type::get_answer_str() const
{
    std::string result;
    std::size_t prev_nth = 0;
    for (process_type const & process : processes) {
        std::string line;

        // 前回の石から順番が飛んでいる場合はパスした場合とみなす
        if (static_cast<int>(prev_nth + 1) == process.stone.nth) {
            line += std::to_string(process.position.x)
                  + " "
                  + std::to_string(process.position.y)
                  + " "
                  + (process.stone.get_side() == stone_type::Sides::Head ? "H" : "T")
                  + " "
                  + std::to_string(process.stone.get_angle() * 90);
        }

        result.append(line);
        result.append("\r\n");
        prev_nth = process.stone.nth;
    }

    return result;
}

// アルゴリズムの基底クラス
// テンプレートのほうがいい?
class SHARED_EXPORT algorithm_type
{
    public:
        algorithm_type(problem_type);
        virtual ~algorithm_type() = 0;
};

#endif // TAKAO_HPP
