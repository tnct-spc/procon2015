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

typedef std::array<std::array<uint8_t,8>,8> raw_stone_type;

using namespace std::string_literals;

// SHARED_EXPORT って書けば外から見える

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
    private:
        raw_stone_type raw_data;

    public:
        stone_type() = default;

        ~stone_type() = default;

        //生配列へのアクセサ
        //座標を受け取ってそこの値を返す
        uint8_t at(size_t y,size_t x)
        {
            return raw_data.at(y).at(x);
        }

        uint8_t at(size_t y,size_t x) const
        {
            return raw_data.at(y).at(x);
        }

        //石へのアクセサ
        //生配列への参照を返す
        raw_stone_type const& get_array()
        {
            return raw_data;
        }

        //時計回りを正方向として指定された角度だけ回転する
        // 自身への参照を返す
        stone_type& rotate(int angle)
        {
            stone_type return_data;

            switch ((angle + 360)/90)
            {
            case 0:
               break;

            case 1:
                for(int i=0;i<8;i++) for(int j=0;j<8;j++)
                {
                    return_data.raw_data[i][j] = raw_data[j-3][i];
                }
                raw_data = return_data.raw_data;
                break;

            case 2:
                for(auto& each_raw_data:raw_data)
                {
                    std::reverse(each_raw_data.begin(),each_raw_data.end());
                }
                std::reverse(raw_data.begin(),raw_data.end());
                break;

            case 3:
                for(int i = 0;i < 8;i++) for(int j = 0;j < 8;j++)
                {
                    return_data.raw_data[i][j] = raw_data[j][3-i];
                }
                raw_data = return_data.raw_data;
                break;

            default:
                break;
            }
            return *this;
        }

        //左右に反転する
        //自身への参照を返す
        stone_type& flip()
        {
            for(auto& each_raw_data:raw_data)
            {
                std::reverse(each_raw_data.begin(),each_raw_data.end());
            }
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
};

// 敷地に置かれた石の情報
class SHARED_EXPORT placed_stone_type
{
    public:
        placed_stone_type() = default;
        ~placed_stone_type() = default;

        stone_type& stone;
        point_type p_in_field;
        point_type p_in_stone;

};

// 敷地
class SHARED_EXPORT field_type
{
    std::array<std::array<uint8_t,40>,40> raw_data;
    public:
        field_type() = default;
        ~field_type() = default;

        //現在の状態における得点を返す
        size_t get_score()
        {
            size_t sum = 0;
            for(size_t i = 8;i < 40;++i)
            {
                sum += std::count(raw_data.at(i).begin()+8,raw_data.at(i).end(),0);
            }
            return sum;
        }

        //石を置く  自身への参照を返す   失敗したら例外を出す
        field_type& put_stone(stone_type const& stone, int y, int x)
        {
            for(int i = 0;i < 8;++i)
            {
                for(int j = 0;j < 8;++j)
                {
                    if(raw_data.at(i+y).at(j+x) == 0) raw_data.at(i+y).at(j+x) = stone.at(i,j);
                    else if(stone.at(i,j) == 0) continue;
                    else std::runtime_error("Failed to put the stone.");
                }
            }
            return *this;
        }

        //指定された場所に指定された石が置けるかどうかを返す
        bool is_puttable(stone_type const& stone, int y, int x)
        {
            for(int i = 0;i < 8;++i)
            {
                for(int j = 0;j < 8;++j)
                {
                    if(raw_data.at(i+y).at(j+x) == 0) continue;
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

        }

         //指定された石を取り除けるかどうかを返す
        bool is_removable(stone_type const& stone)
        {

        }

        //置かれた石の一覧を表す配列を返す
        std::vector<stone_type>& list_of_stones() const
        {

        }
};
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
        static std::vector<std::string> _split(std::string const & target, std::string const & delimiter);
};

problem_type::problem_type(std::string const & problem_text)
{
    auto && split = _split_problem_text(problem_text);
    auto & field_text = std::get<0>(split);
    auto & stone_texts = std::get<1>(split);

    // field = field_type(field_text);
    for (auto const & stone_text : stone_texts) {
        // stones.emplace_back(stone_text);
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

std::vector<std::string> problem_type::_split(std::string const & target, std::string const & delimiter)
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

// 解答データの手順ひとつ分
class SHARED_EXPORT process_type
{
    public:
        process_type() = default;
        ~process_type() = default;
};

// 解答データ
class SHARED_EXPORT answer_type
{
    public:
        answer_type() = default;
        ~answer_type() = default;
};

// アルゴリズムの基底クラス
// テンプレートのほうがいい?
class SHARED_EXPORT algorithm_type
{
    public:
        algorithm_type(problem_type);
        virtual ~algorithm_type() = 0;
};

#endif // TAKAO_HPP
