#ifndef DATA_TYPE_HPP
#define DATA_TYPE_HPP
#include <string>
#include <vector>
#include <tuple>
#include "field_type.hpp"
#include "stone_type.hpp"
//-----------------------------------------------------------
// 問題データ
class problem_type
{
    public:
        problem_type() = default;
        ~problem_type() = default;

        problem_type(std::string const & problem_text);
        problem_type(field_type const& field_, std::vector<stone_type> const& stones_);

        field_type field;
        std::vector<stone_type> stones;

        std::string str();

    private:
        static std::tuple<std::string, std::vector<std::string>> _split_problem_text(std::string const & problem_text);
};

#endif // DATA_TYPE_HPP
