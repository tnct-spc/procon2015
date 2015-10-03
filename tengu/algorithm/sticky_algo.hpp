#ifndef STICKY_ALGO_HPP
#define STICKY_ALGO_HPP
#include <takao.hpp>
class field_with_score_type{
public:
    field_type field;
    double score;
};
class eval_val{
public:
    double score;
    bool should_pass;
};
class sticky_algo : public algorithm_type
{
    Q_OBJECT
public:
    sticky_algo(problem_type _problem);
    ~sticky_algo();
    void run();

private:
    const evaluator _evaluator;
    const problem_type origin_problem;
    problem_type problem;
    std::vector<field_with_score_type> eval_pattern(stone_type stone, stone_type next_stone, bool non_next_stone, std::vector<field_with_score_type> pattern, int search_width);
    std::vector<field_with_score_type> result_stone;
};

#endif // STICKY_ALGO_HPP
