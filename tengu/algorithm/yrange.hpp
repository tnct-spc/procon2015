#ifndef YRANGE_HPP

#define YRANGE_HPP
#include <takao.hpp>
#include <array>
#include <QElapsedTimer>

class yrange : public algorithm_type
{
    Q_OBJECT
public:
    yrange(problem_type _problem, int time_limit);
    ~yrange();
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

    problem_type pre_problem;
    void one_try(problem_type& problem, std::size_t stone_num);
    int evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    search_type search(field_type& _field, stone_type& stone);
    int get_island(field_type::raw_field_type field);
    bool pass(search_type const& search, stone_type const& stone);

    //Time
    QElapsedTimer limit_timer;
    const int time_limit;
};


#endif // YRANGE_HPP
