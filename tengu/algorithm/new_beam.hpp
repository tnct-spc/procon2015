#ifndef NEW_BEAM_HPP
#define NEW_BEAM_HPP
#include <takao.hpp>
#include "evaluator.hpp"
#include <array>
#include <queue>
#include <memory>
#include <vector>

struct node
{
    std::shared_ptr<node> parent;

    std::size_t stone_num;
    point_type point;
    std::size_t angle;
    stone_type::Sides side;
    double score;
    std::size_t search_depth;

    node(std::shared_ptr<node> _parent, std::size_t _stone_num, point_type _point, std::size_t _angle, stone_type::Sides _side, double _score, std::size_t _search_depth):
        parent(_parent),
        stone_num(_stone_num),
        point(_point),
        angle(_angle),
        side(_side),
        score(_score),
        search_depth(_search_depth)
    {
        //printf("Construct %c\n", c);
    }

    node(node *_parent, std::size_t _stone_num, point_type _point, std::size_t _angle, stone_type::Sides _side, double _score, std::size_t _search_depth):
        parent(_parent),
        stone_num(_stone_num),
        point(_point),
        angle(_angle),
        side(_side),
        score(_score),
        search_depth(_search_depth)
    {
        //printf("Construct %c\n", c);
    }

    ~node()
    {
        //printf("Destruct %c\n", c);
    }
};

class new_beam : public algorithm_type
{
    Q_OBJECT
public:
    new_beam(problem_type _problem);
    new_beam(problem_type _problem,evaluator eval);
    ~new_beam();
    void run();

private:

    evaluator eval = evaluator();
    static constexpr std::size_t MAX_SEARCH_DEPTH = 3;
    static constexpr std::size_t MAX_SEARCH_WIDTH = 3;
    std::size_t ALL_STONES_NUM;
    problem_type origin_problem;
    std::size_t now_put_stone_num = 0;

    std::vector<std::shared_ptr<node>> result_vec;

    void one_try(problem_type problem, int y, int x, std::size_t const angle, const int side);
    void only_one_try(problem_type problem);
    int search(field_type& _field, std::size_t const stone_num, std::shared_ptr<node> parent);
    int get_rem_stone_zk(int stone_num);
};

#endif // NEW_BEAM_HPP
