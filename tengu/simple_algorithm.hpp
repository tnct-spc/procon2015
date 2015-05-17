#ifndef SIMPLE_ALGORITHM_H
#define SIMPLE_ALGORITHM_H

#include <QString>
#include <takao.hpp>

class simple_algorithm
{
public:
    simple_algorithm();
    ~simple_algorithm();
    field_type run(std::string problem_data);
};

#endif // SIMPLE_ALGORITHM_H
