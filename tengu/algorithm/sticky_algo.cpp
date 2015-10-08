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
sticky_algo::sticky_algo(problem_type _problem, evaluator _eval) : _evaluator(_eval), origin_problem(_problem), problem(_problem)
{
    algorithm_name = "sticky";
}
sticky_algo::~sticky_algo(){

}
std::vector<field_with_score_type> sticky_algo::eval_pattern(stone_type& stone,std::vector<field_with_score_type> pattern, int search_width){
    result_stone.clear();
    std::vector<stone_params_type> stone_placement_vector;
    stone_placement_vector.reserve(SEARCH_WIDTH);
    for(field_with_score_type& _eval_field : pattern){
        for(int flip = 0; flip <= 1 ;flip++,stone.flip())for(int angle = 0; angle <= 270;angle += 90,stone.rotate(90))for(int dy=-7;dy<=32;dy++)for(int dx=-7;dx<=32;dx++){
            if(_eval_field.field.is_puttable_basic(stone,dy,dx)){
                double score;
                bool should_pass;
                    should_pass = _evaluator.should_pass(_eval_field.field,origin_problem.stones,bit_process_type(stone.get_nth(),static_cast<stone_type::Sides>(flip),angle,{dy,dx}),get_rem_stone_zk(stone));
                    score = _evaluator.move_goodness(_eval_field.field,origin_problem.stones,
                                                     bit_process_type(stone.get_nth(),
                                                     static_cast<stone_type::Sides>(flip),
                                                     angle,
                                                     {dy,dx}));
                //ビームサーチの幅制限
                if(stone_placement_vector.size() < (std::size_t)search_width){
                    if(should_pass){
                        stone_placement_vector.emplace_back(dy,dx,angle,static_cast<stone_type::Sides>(flip),_eval_field.score,true,&_eval_field.field);
                    }else{
                        stone_placement_vector.emplace_back(dy,dx,angle,static_cast<stone_type::Sides>(flip),score,false,&_eval_field.field);
                    }
                }else{
                    auto worst_stone_param = std::min_element(stone_placement_vector.begin(),stone_placement_vector.end(),[](auto const &t1, auto const &t2){return t1.score < t2.score;});
                    if(should_pass){
                        if(_eval_field.score > worst_stone_param->score){
                            //(*worst_stone_param) = {dy,dx,angle,static_cast<stone_type::Sides>(flip),_eval_field.score,true,&_eval_field.field};
                            (*worst_stone_param) = stone_params_type(dy,dx,angle,static_cast<stone_type::Sides>(flip),_eval_field.score,true,&_eval_field.field);
                        }
                    }else{
                        if(score > worst_stone_param->score){
                            //(*worst_stone_param) = {dy,dx,angle,static_cast<stone_type::Sides>(flip),score,false,&_eval_field.field};
                            (*worst_stone_param) = stone_params_type(dy,dx,angle,static_cast<stone_type::Sides>(flip),score,false,&_eval_field.field);
                        }
                    }
                }
                if(_eval_field.field.is_puttable_basic(stone,dy,dx) == 0){
                    qDebug() << "壊れてる";
                }
            }
        }
    }
    for(stone_params_type& stone_params : stone_placement_vector){
        if(stone_params.pass){
            result_stone.emplace_back(*(stone_params.field),stone_params.score);
        }else{
            field_type field = *(stone_params.field);
            if(field.is_puttable_basic(stone.set_angle(stone_params.process.rotate * 90).set_side(static_cast<stone_type::Sides>(stone_params.process.flip)),stone_params.process.position.y,stone_params.process.position.x) == 0){
                qDebug() << "おかしい";
            }
            result_stone.push_back({field.put_stone_basic(stone.set_angle(stone_params.process.rotate * 90).set_side(static_cast<stone_type::Sides>(stone_params.process.flip)),stone_params.process.position.y,stone_params.process.position.x),stone_params.score});
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
            pattern = eval_pattern(*stone_itr,std::move(pattern),5);
        }else{
            pattern = eval_pattern(*stone_itr,std::move(pattern),5);
        }
    }
    field_with_score_type best_ans = *std::min_element(pattern.begin(),pattern.end(),[](auto  &t1, auto  &t2) {
        return t1.field.get_score() < t2.field.get_score();
    });
    int time = timer.elapsed();
    print_text(std::to_string(time) + "msかかった");
    answer_send(best_ans.field);
}
