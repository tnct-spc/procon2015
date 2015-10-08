#ifndef YRANGE_HPP

#define YRANGE_HPP
#include <takao.hpp>
#include <array>
#include "evaluator.hpp"

class yrange : public algorithm_type
{
    Q_OBJECT
public:
    yrange(problem_type _problem);
    yrange(problem_type _problem, evaluator eval);
    ~yrange();
    void run();

private:
    struct search_type
    {
        point_type point;
        std::size_t angle;
        stone_type::Sides side;
        int score;
        double complexity;
    };

    problem_type pre_problem;
    evaluator eval;
    void one_try(problem_type& problem, int x, int y, std::size_t const rotate);
    search_type search(field_type& _field, stone_type& stone);
    bool pass(search_type const& search, stone_type const& stone);
};


#endif // YRANGE_HPP
