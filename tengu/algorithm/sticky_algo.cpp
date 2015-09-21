#include "sticky_algo.hpp"
#include <limits>
#include <sstream>
#include <algorithm>
#include "QDebug"
#include "QString"
#include "QElapsedTimer"
#include <boost/range/algorithm_ext/erase.hpp>
sticky_algo::sticky_algo(problem_type _problem) : origin_problem(_problem),problem(_problem)
{
    algorithm_name = "sticky";
}
sticky_algo::~sticky_algo(){

}

double sticky_algo::eval(field_type& field,const stone_type& stone, int pos_y, int pos_x){
    double score = 0;
    //int neighbors = 0;
    if(!field.is_puttable(stone,pos_y,pos_x))return -1;
    field.put_stone(stone,pos_y,pos_x);
    score = field.evaluate_normalized_complexity();
    field.remove_large_most_number_and_just_before_stone();
    return score;
}
std::vector<evalated_field> sticky_algo::eval_pattern(stone_type stone, std::vector<evalated_field> pattern, int search_width){
    for(auto _eval_field : pattern){
        for(int flip = 0; flip <= 1 ;flip++,stone.flip())for(int angle = 0; angle < 4;angle++,stone.rotate(90))for(int dy=-7;dy<=32;dy++)for(int dx=-7;dx<=32;dx++){
            double score = eval(_eval_field.field,stone,dy,dx);
            if(score >= 0){
                if(result_stone.size() < search_width){
                    result_stone.push_back(putted_evalated_field{flip,angle*90,dy,dx,std::move(_eval_field),score});
                }else{
                    auto max_result = std::max_element(result_stone.begin(),result_stone.end(),
                                                                         [](auto const &t1, auto const &t2) {
                                                                                 return t1.score < t2.score;
                    });
                    if(max_result->score > score){
                        *max_result = putted_evalated_field{flip,angle*90,dy,dx,std::move(_eval_field),score};
                    }
                }
            }
        }
    }
    int search_size = std::min(static_cast<int>(result_stone.size()),search_width);
    std::vector<evalated_field> return_field_vactor;
    return_field_vactor.reserve(search_size);
    //qDebug() << result_stone.size();
    for(int i = 0; i < search_size; i++){
        //qDebug() << "angle" << result_stone.at(i).angle << "side" << result_stone.at(i).flip << "y" << result_stone.at(i).y << "x" << result_stone.at(i).x;
        result_stone.at(i).e_field.field.put_stone(
              std::move(stone.set_angle(result_stone.at(i).angle).set_side(static_cast<stone_type::Sides>(result_stone.at(i).flip))),
              result_stone.at(i).y,result_stone.at(i).x);
        result_stone.at(i).e_field.score = result_stone.at(i).score;
        return_field_vactor.push_back(std::move(result_stone.at(i).e_field));
    }
    if(result_stone.size() == 0){
        return std::move(pattern);
    }
    result_stone.clear();
    return std::move(return_field_vactor);
}

void sticky_algo::run(){

    QElapsedTimer et;
    et.start();
    int count = problem.stones.size();
    std::vector<evalated_field> pattern;
    pattern.push_back({problem.field,0});
    for(auto& _stone : problem.stones){
        pattern = eval_pattern(std::move(_stone),std::move(pattern),50);
        qDebug() << count--;
    }
    int64_t time = et.elapsed();
    print_text(std::to_string(time));
    evalated_field best_ans = *std::min_element(pattern.begin(),pattern.end(),[](auto  &t1, auto  &t2) {
        return t1.field.get_score() < t2.field.get_score();
    });
    answer_send(best_ans.field);
}
