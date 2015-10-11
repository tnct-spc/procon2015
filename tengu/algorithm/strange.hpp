#ifndef STRANGE_HPP
#define STRANGE_HPP

#include <takao.hpp>

namespace stranger {
class field_with_score_type{
public:
    field_with_score_type(field_type const& field_, double score_) :
        field(field_), score(score_)
    {
    }
    field_type field;
    double score;
};
class stone_params_type{
public:
    stone_params_type(int _dy, int _dx, int _angle, stone_type::Sides _side, double _score, bool _pass, field_type* _field){
        process.position.y = _dy;
        process.position.x = _dx;
        process.angle = _angle ;
        process.flip = static_cast<unsigned int>(_side);
        score = _score;
        pass = _pass;
        field = _field;
    }
    double score;
    bool pass;
    field_type* field;
    bit_process_type process;
};
} // namespace stranger

class strange : public algorithm_type
{
    Q_OBJECT
public:
    strange(problem_type _problem);
    strange(problem_type _problem, evaluator _eval);
    ~strange();
    void run();

private:

    static constexpr std::size_t SEARCH_WIDTH = 1;

    evaluator _evaluator = evaluator(-0.5,0.1,0.3,0.5);
    problem_type origin_problem;
    problem_type problem;
    std::vector<stranger::field_with_score_type> eval_pattern(stone_type& stone, std::vector<stranger::field_with_score_type> pattern, int search_width);
    std::vector<stranger::field_with_score_type> force_put(stone_type& stone, std::vector<stranger::field_with_score_type> pattern, bit_process_type process);

};
#endif // STRANGE_HPP
