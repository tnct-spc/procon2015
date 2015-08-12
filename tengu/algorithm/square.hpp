#ifndef SIMPLE_ALGORITHM_H
#define SIMPLE_ALGORITHM_H

#include <takao.hpp>

class simple_algorithm : public algorithm_type
{
    Q_OBJECT
public:
    simple_algorithm(problem_type _problem);
    ~simple_algorithm();
    void run();

private:
    problem_type pre_problem;
};

#endif // SIMPLE_ALGORITHM_H
