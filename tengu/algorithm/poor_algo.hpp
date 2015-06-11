#ifndef POOR_ALGO_H
#define POOR_ALGO_H

#include <takao.hpp>
class poor_algo :public algorithm_type
{
    Q_OBJECT
public:
    poor_algo(problem_type _problem);
    ~poor_algo();
    void run();
};

#endif // POOR_ALGO_H
