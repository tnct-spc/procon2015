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
    public:
        stone_type() = default;
        ~stone_type() = default;
};

// 敷地に置かれた石の情報
class SHARED_EXPORT placed_stone_type
{
    public:
        placed_stone_type() = default;
        ~placed_stone_type() = default;
};

// 敷地
class SHARED_EXPORT field_type
{
    public:
        field_type() = default;
        ~field_type() = default;
};

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
