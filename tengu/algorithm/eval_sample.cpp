#include "eval_sample.hpp"
#include "evaluator.hpp"

#include <utility> // pair, tuple

// 作成中、全く動かない
eval_sample::eval_sample(problem_type problem_, evaluator eval_) :
    problem(problem_),
    algorithm_name("eval_sample"),
    eval(eval_)
{
}

eval_sample::~eval_sample()
{
}

void eval_sample::run()
{
    auto& field = problem.field;
    auto& stones = problem.stones;
    for(auto sit = stones.begin(); sit != stones.end(); sit++) {
        // 最後の石は特別
        if(sit == stones.end() - 1) {
            ;
            break;
        }

        // 各点のprocessと評価値のpairのvector
        std::vector<std::pair<process_type, double>> candidates;
        for(auto pit = all_points.begin(); pit != all_points.end(); pit++) {
            if(field.is_puttable(stone, pit->y, pit->x)) {
                if(!eval.should_pass(field, process_type(stone, )))
            candidates[i] = std::make_tuple(process_type(*sit, *pit), move_goodness;
        }
}
