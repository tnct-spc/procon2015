#ifndef YRANGE_BASED_YAYOI_HPP

#define YRANGE_BASED_YAYOI_HPP
#include <takao.hpp>
#include <array>
#include <QElapsedTimer>

class yrange_based_yayoi : public algorithm_type
{
    Q_OBJECT
public:
    yrange_based_yayoi(problem_type _problem, int time_limit);
    ~yrange_based_yayoi();
    void run();

private:
    struct search_type
    {
        point_type point;
        std::size_t angle;
        stone_type::Sides side;
        int score;
        int island;
    };

    problem_type origin_problem;
    void solve();
    void one_try(problem_type& problem, std::size_t stone_num);
    int evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    search_type search(field_type& _field, stone_type& stone);
    search_type search_when_second(field_type& _field, stone_type& stone);
    int get_island(field_type::raw_field_type field);
    bool pass(search_type const& search, stone_type const& stone);
    double degree_of_exposure(int score, stone_type const& stone);

    //時間切れ後に細かい置き直しを試みるためのプロセス
    void improve();
    std::vector<process_type> best_processes;
    int64_t best_field[64];

    //Time
    QElapsedTimer limit_timer;
    const int time_limit;
};


#endif // YRANGE_BASED_YAYOI_HPP
