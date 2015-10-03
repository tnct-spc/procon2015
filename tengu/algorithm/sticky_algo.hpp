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
    stone_params_type(int dy,int dx,int angle,stone_type::Sides side,double score,bool pass,field_type* field):dy(dy),dx(dx),angle(angle),side(side),score(score),pass(pass),field(field){}
    /*
    stone_params_type& operator=(stone_params_type& lval){
        dy = lval.dy;
        dx = lval.dx;
        angle = lval.angle;
        side = lval.side;
        score = lval.score;
        pass = lval.pass;
        field = lval.field;
        return *this;
    }
    */
    int dy;
    int dx;
    int angle;
    stone_type::Sides side;
    double score;
    bool pass;
    field_type* field;
};

class sticky_algo : public algorithm_type
{
    Q_OBJECT
public:
    sticky_algo(problem_type _problem);
    sticky_algo(problem_type _problem, evaluator _eval);
    ~sticky_algo();
    void run();

private:
    evaluator _evaluator = evaluator(-10,1,1,0.5);
    double light_eval(field_type& field,process_type process);
    const problem_type origin_problem;
    problem_type problem;
    std::vector<field_with_score_type> eval_pattern(stone_type& stone,stone_type& next_stone, bool non_next_stone, std::vector<field_with_score_type> pattern, int search_width);
    std::vector<field_with_score_type> result_stone;

};

#endif // STICKY_ALGO_HPP
