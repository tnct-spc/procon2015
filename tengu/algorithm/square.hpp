#ifndef SQUARE_H
#define SQUARE_H

#include <takao.hpp>

class square : public algorithm_type
{
    Q_OBJECT
public:
    square(problem_type _problem);
    ~square();
    void run();

private:
    problem_type pre_problem;

    void solve(int mode,auto stone);
    void put_stone(auto,int);
};

#endif // SQUARE_H
