#ifndef YRANGE_HPP
#define YRANGE_HPP
#include <takao.hpp>
#include <array>
int constexpr FIELD_SIZE = 32;

struct yrange_type
{
    field_type field;
    stone_type stone;
    point_type point;
    double value;
    int angle;
    int flip;
    point_type first_point;
};

struct search_type
{
    point_type point;
    int rotate;
    int flip;
    int score;
};

class yrange : public algorithm_type
{
    Q_OBJECT
public:
    yrange(problem_type _problem);
    ~yrange();
    void run();

private:
    problem_type pre_problem;
    int evaluate(field_type const& field, stone_type stone)const;
    search_type search(field_type& _field, stone_type const& _stone);
};


#endif // YRANGE_HPP

