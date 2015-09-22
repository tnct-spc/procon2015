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
        point_type point;
        std::size_t angle;
        stone_type::Sides side;
        int score;
        int island;
    };
    struct locale_search_type
    {
        point_type point;
        std::size_t rotate;
        stone_type::Sides side;
        int score;
    };

    problem_type pre_problem;
    std::size_t STONE_NUM;

    int evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    bool search(field_type& _field, stone_type& stone);
    int get_island(field_type::raw_field_type field);
    bool pass(search_type const& search, stone_type const& stone);
    bool pass(locale_search_type const& search, stone_type const& stone);
    bool local_put(field_type& field, stone_type& stone, int _y, int _x);
};
#endif // YRANGE2_HPP
