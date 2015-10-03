#include "sticky_algo.hpp"
#include <limits>
#include <sstream>
#include <algorithm>
#include "QDebug"
#include "QString"
#include "QElapsedTimer"
#include <boost/range/algorithm_ext/erase.hpp>
#include "field_type.hpp"
sticky_algo::sticky_algo(problem_type _problem) : origin_problem(_problem),problem(_problem)
{
    algorithm_name = "sticky";
}
sticky_algo::~sticky_algo(){

}

double sticky_algo::eval(field_type& field,const stone_type& stone, int pos_y, int pos_x){
    double score = 0;
    //int neighbors = 0;
    if(!field.is_puttable_basic(stone,pos_y,pos_x))return -1;
    field.put_stone_basic(stone,pos_y,pos_x);
    score = field.evaluate_normalized_complexity();
    field.remove_stone_basic();
    return score;
}
std::vector<field_with_score_type> sticky_algo::eval_pattern(stone_type stone, std::vector<field_with_score_type> pattern, int search_width){
    result_stone.clear();
    for(field_with_score_type _eval_field : pattern){
        for(int flip = 0; flip <= 1 ;flip++,stone.flip())for(int angle = 0; angle < 4;angle++,stone.rotate(90))for(int dy=-7;dy<=32;dy++)for(int dx=-7;dx<=32;dx++){
            double score = eval(_eval_field.field,stone,dy,dx);
            bool should_pass;
            //std::numeric_limits<double>::min()が無効値
            if(score == std::numeric_limits<double>::min())continue;
            if(result_stone.size() < search_width){
                if(should_pass){
                    result_stone.push_back({_eval_field.field,_eval_field.score});
                }else{
                    field_type _field = _eval_field.field;
                    result_stone.push_back({_field.put_stone_basic(stone,dy,dx),score});
                }
            }else{
                auto worst_score_field = std::max_element(result_stone.begin(),result_stone.end(),[](auto const &t1, auto const &t2){return t1.score < t2.score;});
                if(should_pass){
                    if(_eval_field.score < worst_score_field->score){
                        *worst_score_field = {_eval_field.field,_eval_field.score};
                    }
                }else{
                    if(score < worst_score_field->score){
                        field_type _field = _eval_field.field;
                        *worst_score_field = {_field.put_stone_basic(stone,dy,dx),score};
                    }
                }
            }
        }
    }
    if(result_stone.size() == 0)return std::move(pattern);
    return result_stone;
}

void sticky_algo::run(){
    std::vector<field_with_score_type> pattern;
    pattern.push_back({problem.field,0});
    for(auto& _stone : problem.stones){
        pattern = eval_pattern(std::move(_stone),std::move(pattern),20);
    }
    field_with_score_type best_ans = *std::min_element(pattern.begin(),pattern.end(),[](auto  &t1, auto  &t2) {
        return t1.field.get_score() < t2.field.get_score();
    });
    answer_send(best_ans.field);
}
