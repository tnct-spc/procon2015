#include "sticky_algo.hpp"
#include <limits>
#include <sstream>
#include <algorithm>
#include "QDebug"
#include "QString"
#include "QElapsedTimer"
#include <boost/range/algorithm_ext/erase.hpp>
#include "field_type.hpp"
#include "QtConcurrent/QtConcurrent"
sticky_algo::sticky_algo(problem_type _problem) : origin_problem(_problem),problem(_problem)
{
    algorithm_name = "sticky";
}
sticky_algo::sticky_algo(problem_type _problem, evaluator _eval) : origin_problem(_problem),problem(_problem),_evaluator(_eval)
{
    algorithm_name = "sticky";
}
sticky_algo::~sticky_algo(){

}
std::vector<field_with_score_type> sticky_algo::eval_pattern( stone_type& stone, stone_type& next_stone, bool non_next_stone, std::vector<field_with_score_type> pattern, int search_width){
    result_stone.clear();
    std::vector<stone_params_type> stone_placement_vector;
    stone_placement_vector.reserve(SEARCH_WIDTH);
    for(field_with_score_type& _eval_field : pattern){
        for(int flip = 0; flip <= 1 ;flip++,stone.flip())for(int angle = 0; angle <= 270;angle += 90,stone.rotate(90))for(int dy=-7;dy<=32;dy++)for(int dx=-7;dx<=32;dx++){
            if(_eval_field.field.is_puttable_basic(stone,dy,dx)){
                double score;
                bool should_pass;
                    if(non_next_stone){
                        //score = _evaluator.move_goodness(_eval_field.field,process_type(stone,{dy,dx}));
                        score = light_eval(_eval_field.field,process_type(stone,{dy,dx}));
                        should_pass = false;
                    }else{
                        //score = _evaluator.move_goodness(_eval_field.field,process_type(stone,{dy,dx}),next_stone);
                        score = light_eval(_eval_field.field,process_type(stone,{dy,dx}));
                        //should_pass = _evaluator.should_pass(_eval_field.field,process_type(stone,{dy,dx}),get_rem_stone_zk(stone));
                        should_pass = false;
                    }
                //ビームサーチの幅制限
                if(stone_placement_vector.size() < search_width){
                    if(should_pass){
                        stone_placement_vector.emplace_back(dy,dx,angle,static_cast<stone_type::Sides>(flip),_eval_field.score,true,&_eval_field.field);
                    }else{
                        stone_placement_vector.emplace_back(dy,dx,angle,static_cast<stone_type::Sides>(flip),score,false,&_eval_field.field);
                    }
                }else{
                    auto worst_stone_param = std::min_element(stone_placement_vector.begin(),stone_placement_vector.end(),[](auto const &t1, auto const &t2){return t1.score < t2.score;});
                    if(should_pass){
                        if(_eval_field.score > worst_stone_param->score){
                            (*worst_stone_param) = {dy,dx,angle,static_cast<stone_type::Sides>(flip),_eval_field.score,true,&_eval_field.field};
                        }
                    }else{
                        if(score > worst_stone_param->score){
                            (*worst_stone_param) = {dy,dx,angle,static_cast<stone_type::Sides>(flip),score,false,&_eval_field.field};
                        }
                    }
                }
            }
        }
    }
    for(stone_params_type& stone_params : stone_placement_vector){
        if(stone_params.pass){
            result_stone.emplace_back(*(stone_params.field),stone_params.score);
        }else{
            field_type field = *(stone_params.field);
            result_stone.push_back({field.put_stone_basic(stone.set_angle(stone_params.angle).set_side(stone_params.side),stone_params.dy,stone_params.dx),stone_params.score});
        }
    }
    if(result_stone.size() == 0)return std::move(pattern);
    return std::move(result_stone);
}

void sticky_algo::run(){
    QElapsedTimer timer;
    timer.start();
    std::vector<field_with_score_type> pattern;
    pattern.emplace_back(problem.field,0);
    size_t cnt = problem.stones.size();
    for(auto stone_itr = problem.stones.begin();stone_itr != problem.stones.end();stone_itr++){
        //最後の石の時
        print_text(std::to_string(cnt--));
        if(stone_itr + 1 == problem.stones.end()){
            pattern = eval_pattern(*stone_itr,*stone_itr,true,std::move(pattern),100);
        }else{
            pattern = eval_pattern(*stone_itr,*(stone_itr+1),false,std::move(pattern),100);
        }
    }
    field_with_score_type best_ans = *std::min_element(pattern.begin(),pattern.end(),[](auto  &t1, auto  &t2) {
        return t1.field.get_score() < t2.field.get_score();
    });
    int time = timer.elapsed();
    print_text(std::to_string(time) + "msかかった");
    answer_send(best_ans.field);
}
double sticky_algo::light_eval(field_type &field, process_type process){
    field.put_stone_basic(process.stone,process.position.y,process.position.x);
    double score = -field.evaluate_normalized_complexity();
    field.remove_stone_basic();
    return score;
}
