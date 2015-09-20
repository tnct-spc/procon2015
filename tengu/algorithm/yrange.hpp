#ifndef YRANGE_HPP

#define YRANGE_HPP
#include <takao.hpp>
#include <array>

class yrange : public algorithm_type
{
    Q_OBJECT
public:
    yrange(problem_type _problem);
    ~yrange();
    void run();

private:
    struct search_type
    {
        point_type point;
        std::size_t rotate;
        stone_type::Sides flip;
        int score;
    };

    problem_type pre_problem;
    std::size_t one_try(problem_type& problem, int x, int y, std::size_t const rotate);
    int evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    search_type search(field_type& _field, stone_type& stone);
    int get_island(field_type::raw_field_type field, point_type const& point);
    bool pass(search_type const& search, stone_type const& stone);
    bool pass(field_type const& field, search_type const& search, int stone_num);
};


#endif // YRANGE_HPP

