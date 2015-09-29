/*
 * sample algorithm using evaluator
 */
#ifndef EVAL_SAMPLE_HPP
#define EVAL_SAMPLE_HPP

#include <takao.hpp>
#include "evaluator.hpp"

class eval_sample : public algorithm_type
{
    Q_OBJECT
public:
    eval_sample(problem_type problem_);
    ~eval_sample();
    void run();
private:
    const std::vector<point_type> all_points = [] {
        std::vector<point_type> v;
        for (int i = -7; i < 32; i++)
            for (int j = -7; j < 32; j++)
                v.push_back(point_type(i, j));
        return v;
    }();
};

#endif // EVAL_SAMPLE_HPP

