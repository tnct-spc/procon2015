#include "answer_type.hpp"

/* コンストラクタ(配給された石の数を取る) */
answer_type::answer_type(int stones)
{
    answer.resize(stones);
    for (auto &p : answer)
        p = boost::optional<process_type>();
}

void answer_type::append(process_type process)
{
    answer.at(process.stone.get_nth()) = process;
}

/* 解答文字列生成 */
std::string answer_type::answer_str()
{
    std::string ret;
    for (auto p : answer) {
        if (p) {
            std::stringstream ss;
            ss << p->position.x << " " << p->position.y << " "
               << (p->stone.get_side() == stone_type::Sides::Head ? "H" : "T")
               << " " << p->stone.get_angle();
            ret.append(ss.str());
        }
        ret.append("\r\n");
    }
    return ret;
}

