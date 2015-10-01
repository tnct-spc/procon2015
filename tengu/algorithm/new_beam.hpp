#ifndef NEW_BEAM_HPP
#define NEW_BEAM_HPP
#include <takao.hpp>
#include "evaluator.hpp"
#include <array>
#include <queue>

class new_beam : public algorithm_type
{
    Q_OBJECT
public:
    new_beam(problem_type _problem);
    ~new_beam();
    void run();

private:

    evaluator eval = evaluator(-10,1,1,0.5);
    static constexpr std::size_t MAX_SEARCH_DEPTH = 3;
    std::size_t ALL_STONES_NUM;
    problem_type origin_problem;

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
        int search_depth = 0;

        search_type(std::vector<stones_info_type> stones_info_vec,double score):stones_info_vec(stones_info_vec),score(score){}
        search_type(std::vector<stones_info_type> stones_info_vec,double score, std::size_t search_depth_):stones_info_vec(stones_info_vec),score(score),search_depth(search_depth_){}
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

    void one_try(problem_type problem, int y, int x, std::size_t const angle, const int side);
    void only_one_try(problem_type problem);
    int search(std::vector<search_type>& sv, search_type s, field_type &_field, std::size_t const stone_num);
    int get_rem_stone_zk(int stone_num);
};

#endif // NEW_BEAM_HPP
