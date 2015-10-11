#ifndef STICKY_ALGO_HPP
#define STICKY_ALGO_HPP
#include <takao.hpp>

class field_with_score_type{
public:
    field_with_score_type(field_type& field,double score):field(field),score(score){}
    field_type field;
    double score;
};
class stone_params_type{
public:
    stone_params_type(int _dy,int _dx,int _angle,stone_type::Sides _side,double _score,bool _pass,field_type* _field){
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

class sticky_algo : public algorithm_type
{
    Q_OBJECT
public:
    sticky_algo(problem_type _problem);
    sticky_algo(problem_type _problem, evaluator _eval);
    sticky_algo(problem_type _problem, std::string algo_name, evaluator _eval);
    ~sticky_algo();
    void run();

private:

    static constexpr std::size_t SEARCH_WIDTH = 100;

    evaluator _evaluator = evaluator(-0.5,0.1,0.3,0.5);
    problem_type origin_problem;
    problem_type problem;
    std::vector<field_with_score_type> eval_pattern(stone_type& stone, std::vector<field_with_score_type> pattern, int search_width);
    std::vector<field_with_score_type> result_stone;

};

#endif // STICKY_ALGO_HPP
