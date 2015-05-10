#ifndef DATA_TYPE_HPP
#define DATA_TYPE_HPP

#include <string>
#include <vector>
#include <tuple>

#include "field_type.hpp"

//-----------------------------------------------------------
// 問題データ
class problem_type
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

#endif // DATA_TYPE_HPP
