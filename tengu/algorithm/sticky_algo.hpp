#ifndef STICKY_ALGO_HPP
#define STICKY_ALGO_HPP
#include <takao.hpp>

class sticky_algo : public algorithm_type
{
    Q_OBJECT
public:
    sticky_algo(problem_type _problem);
    ~sticky_algo();
    void run();

private:
    const problem_type origin_problem;
    problem_type problem;
    int eval(field_type &field, stone_type &stone, int pos_y, int pos_x);
};

#endif // STICKY_ALGO_HPP
