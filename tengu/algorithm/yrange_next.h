#ifndef YRANGENEXT_HPP

#define YRANGENEXT_HPP
#include <takao.hpp>
#include <array>
#include <QElapsedTimer>
#include "evaluator.hpp"

class yrange_next : public algorithm_type
{
    Q_OBJECT
public:
    yrange_next(problem_type _problem, int time_limit, evaluator _eval, int _mode);
    ~yrange_next();
    void run();

    int const mode;

private:
    struct search_type
    {
        point_type point;
        std::size_t angle;
        stone_type::Sides side;
        double score;
        int island_num;
    };

    problem_type origin_problem;
    evaluator eval;
    void solve();
    void one_try(field_type &field, std::size_t stone_num);
    search_type search(field_type& _field, stone_type& stone);
    bool pass(search_type const& search);
    int count_island_fast(field_type const& field, int& one_island_num);

    //Time
    //QElapsedTimer limit_timer;
    //const int time_limit;
    int emit_count=0;
    int best_score = FIELD_SIZE * FIELD_SIZE;

    int processes_size_sum = 0;
    int processes_size_ave = -1;
    int processes_cut_line = 256;
};


#endif // YRANGENEXT_HPP
