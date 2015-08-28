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
    int threshold_zk;
    problem_type pre_problem;
    double evaluate(yrange_type const& one)const;
    void place(field_type& field, int const m, int const n);
    int evaluate(stone_type const stone, int const x, int const y)const;
    search_type search(field_type& _field, stone_type const& _stone);

};


#endif // YRANGE_HPP

