#ifndef READAHEAD
#define READAHEAD
#include "takao.hpp"
#include "evaluator.hpp"
#include <array>
#include <queue>

class read_ahead : public algorithm_type
{
    Q_OBJECT
public:
    read_ahead(problem_type _problem, evaluator _eval);
    ~read_ahead();
    void run();

    struct stones_info_type
    {
        point_type point;
        std::size_t angle;
        stone_type::Sides side;
        stones_info_type(point_type point, std::size_t angle, stone_type::Sides side):point(point),angle(angle),side(side){}
        stones_info_type(){}
    };

    struct search_type
    {
        std::vector<stones_info_type> stones_info_vec;
        double score = -1;
        double complexity = -1;
        search_type(std::vector<stones_info_type> stones_info_vec,double score,double complexity):stones_info_vec(stones_info_vec),score(score),complexity(complexity){}
        search_type(){}
        friend inline bool operator== (search_type const& lhs, search_type const& rhs)
        {
            return lhs.stones_info_vec[0].point == rhs.stones_info_vec[0].point &&
                   lhs.stones_info_vec[0].angle == rhs.stones_info_vec[0].angle &&
                   lhs.stones_info_vec[0].side == rhs.stones_info_vec[0].side &&
                   lhs.stones_info_vec.size() == rhs.stones_info_vec.size() &&
                   lhs.score == rhs.score;
        }

    };

private:

    std::size_t search_depth = 3;
    std::size_t ALL_STONES_NUM;
    problem_type pre_problem;
    evaluator eval;
    void one_try(problem_type problem, int y, int x, std::size_t const angle, const int side);
    int search(std::vector<search_type>& sv, search_type s, field_type &_field, std::size_t const stone_num);
    bool pass(search_type const& search);
};

#endif // READAHEAD
