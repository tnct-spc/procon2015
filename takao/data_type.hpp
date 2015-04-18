#ifndef TAKAO_HPP
#define TAKAO_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <string>
#include <tuple>
#include <vector>
#include <array>
#include <cstdint>
#include <algorithm>

#include "takao_global.hpp"

#include "point_type.hpp"
#include "stone_type.hpp"
#include "placed_stone_type.hpp"
#include "field_type.hpp"


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
    for(std::size_t i = 0; i < stone_texts.size(); ++i)
    {
         stones.emplace_back(stone_texts[i], i);
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
