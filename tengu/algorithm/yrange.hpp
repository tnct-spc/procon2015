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

class yrange : public algorithm_type
{
    Q_OBJECT
public:
    yrange(problem_type _problem);
    ~yrange();
    void run();

private:
    problem_type pre_problem;
    double evaluate(yrange_type const& one)const;
};


#endif // YRANGE_HPP

