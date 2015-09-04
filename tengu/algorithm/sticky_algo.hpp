#ifndef STICKY_ALGO_HPP
#define STICKY_ALGO_HPP
#include <takao.hpp>
struct evalated_field{
    field_type field;
    int score;
};
struct putted_evalated_field{
    int flip;
    int angle;
    int y;
    int x;
    evalated_field e_field;
    int score;
};

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
    int eval(field_type &field, const stone_type &stone, int pos_y, int pos_x);
    std::vector<evalated_field> eval_pattern(stone_type stone, std::vector<evalated_field> pattern, int search_width);
};

#endif // STICKY_ALGO_HPP
