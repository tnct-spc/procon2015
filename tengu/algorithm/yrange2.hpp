#ifndef YRANGE2_HPP
#define YRANGE2_HPP

#include <takao.hpp>
#include <array>

class yrange2 : public algorithm_type
{
    Q_OBJECT
public:
    yrange2(problem_type _problem);
    ~yrange2();
    void run();

private:
    struct search_type
    {
        field_type field;
        point_type point;
        std::size_t rotate;
        stone_type::Sides flip;
        int score;
        int island;
    };

    problem_type pre_problem;
    std::size_t STONE_NUM;

    void one_try(problem_type& problem, int x, int y, std::size_t const rotate);
    int evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    std::vector<search_type> search(field_type& _field, stone_type& stone);
    std::vector<search_type> search2(search_type& s, stone_type& stone);
    int get_island(field_type::raw_field_type field);
    bool pass(search_type const& search, stone_type const& stone);
};
#endif // YRANGE2_HPP
