#ifndef STICKY_BEAM_HPP
#define STICKY_BEAM_HPP
#include <takao.hpp>
#include "evaluator.hpp"
#include <array>
#include <queue>
#include <memory>
#include <vector>

class sticky_beam : public algorithm_type
{
    Q_OBJECT

    struct node
    {
        std::shared_ptr<node> parent;

        std::size_t stone_num;
        point_type point;
        std::size_t angle;
        stone_type::Sides side;
        double score;
        std::size_t search_depth;

        node(std::shared_ptr<node> _parent, std::size_t _stone_num, point_type _point, std::size_t _angle, stone_type::Sides _side, double _score, std::size_t _search_depth):
            parent(_parent),
            stone_num(_stone_num),
            point(_point),
            angle(_angle),
            side(_side),
            score(_score),
            search_depth(_search_depth)
        {}

        node(node *_parent, std::size_t _stone_num, point_type _point, std::size_t _angle, stone_type::Sides _side, double _score, std::size_t _search_depth):
            parent(_parent),
            stone_num(_stone_num),
            point(_point),
            angle(_angle),
            side(_side),
            score(_score),
            search_depth(_search_depth)
        {}

        ~node()
        {}
    };

    class field_with_score_type{
    public:
        field_with_score_type(field_type& field,double score):field(field),score(score){}
        field_type field;
        double score;
    };

public:
    sticky_beam(problem_type _problem);
    sticky_beam(problem_type _problem,evaluator eval);
    ~sticky_beam();
    void run();

private:

    evaluator eval = evaluator();
    static constexpr std::size_t MAX_SEARCH_DEPTH = 15;
    static constexpr std::size_t MAX_SEARCH_WIDTH = 3;
    std::size_t ALL_STONES_NUM;
    problem_type origin_problem;
    std::size_t now_put_stone_num = 0;

    std::vector<field_with_score_type> holding_fields;
    std::vector<std::shared_ptr<node>> result_vec;

    void one_try(problem_type problem, int y, int x, std::size_t const angle, const int side);
    void only_one_try(problem_type problem);
    int search(field_type& _field, std::size_t const stone_num, std::shared_ptr<node> parent);

    //sticky
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

    double light_eval(field_type& field,process_type process);
    problem_type problem;
    void eval_pattern(stone_type& stone, stone_type& next_stone);
    std::vector<field_with_score_type> result_stone;


};

#endif // STICKY_BEAM_HPP
