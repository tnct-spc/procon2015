#ifndef YRANGE_HPP

#define YRANGE_HPP
#include <takao.hpp>
#include <array>
#include <QElapsedTimer>
#include "evaluator.hpp"

class yrange : public algorithm_type
{
    Q_OBJECT
public:
    yrange(problem_type _problem, int time_limit, evaluator _eval);
    ~yrange();
    void run();

private:
    struct search_type
    {
        point_type point;
        std::size_t angle;
        stone_type::Sides side;
        double score;
        double complexity;
    };

    problem_type origin_problem;
    evaluator eval;
    void solve();
    void one_try(field_type &field, std::size_t stone_num);
    search_type search(field_type& _field, stone_type& stone);
    bool pass(search_type const& search);

    //Time
    QElapsedTimer limit_timer;
    const int time_limit;
    int emit_count=0;
    int best_score = FIELD_SIZE * FIELD_SIZE;
};


#endif // YRANGE_HPP
