#include "problem_type.hpp"
#include "utils.hpp"
problem_type::problem_type(std::string const & problem_text)
{
    auto && split = _split_problem_text(problem_text);
    auto & field_text = std::get<0>(split);
    auto & stone_texts = std::get<1>(split);

    field = field_type(field_text,stone_texts.size());
    for(std::size_t i = 0; i < stone_texts.size(); ++i)
    {
         stones.emplace_back(stone_texts[i], i+1/*一つ目の石のnthは1(1st) iつ目でi+1*/);
    }
}

std::tuple<std::string, std::vector<std::string>> problem_type::_split_problem_text(std::string const & problem_text)
{
    std::tuple<std::string, std::vector<std::string>> result;
    auto & field = std::get<0>(result);
    auto & stones = std::get<1>(result);

    auto split = _split(problem_text, "\r\n\r\n");

    field = split.front();

    std::copy(std::next(split.begin()),
              split.end(),
              std::back_inserter(stones));
    stones.front().erase(0, stones.front().find('\n') + 1);

    return result;
}

