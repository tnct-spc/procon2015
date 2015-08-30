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
        int rotate;
        int flip;
        int score;
    };

    problem_type pre_problem;
    int evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    yrange::search_type search(field_type& _field, stone_type const& _stone);
};


#endif // YRANGE_HPP

