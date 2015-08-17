#ifndef ANSWER_TYPE_HPP
#define ANSWER_TYPE_HPP

/* 
 * takao/answer_type.hpp
 * 解答データの保持と、解答文字列の書き出し・読み込み
 * 今のところproblem_makerでしか使わない模様？
 */

#include <vector>
#include <sstream>
#include <boost/optional.hpp>
#include "stone_type.hpp"
#include "point_type.hpp"
#include "field_type.hpp" /* for process_type */

class answer_type
{
private:
    std::vector<boost::optional<process_type>> answer;
    
public:
    answer_type(int stones); /* 問題で渡される石の総数 */
    void append(process_type process);
    std::string answer_str();
};

#endif // ANSWER_TYPE_HPP

