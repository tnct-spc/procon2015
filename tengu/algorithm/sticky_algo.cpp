#include "sticky_algo.hpp"
#include <limits>
#include <sstream>
#include "QDebug"
#include "QString"
sticky_algo::sticky_algo(problem_type _problem) : origin_problem(_problem),problem(_problem)
{
    algorithm_name = "sticky";
}
sticky_algo::~sticky_algo(){

}

int sticky_algo::eval(field_type& field, stone_type& stone, int pos_y, int pos_x){
    int score = 0;
    int neighbors = 0;
    if(!field.is_puttable(stone,pos_y,pos_x))return -1;
    for(int i = 0; i < 8;i ++)for(int j = 0; j < 8 ; j ++){
        if(stone.get_raw_data().at(i).at(j) == 1){
            if(pos_y + i + 1 < 32 && field.get_raw_data().at(pos_y + i + 1).at(pos_x + j))neighbors ++;
            if(pos_y + i - 1 >= 0 && field.get_raw_data().at(pos_y + i - 1).at(pos_x + j))neighbors ++;
            if(pos_x + j + 1 < 32 && field.get_raw_data().at(pos_y + i).at(pos_x + j + 1))neighbors ++;
            if(pos_x + j - 1 >= 0 && field.get_raw_data().at(pos_y + i).at(pos_x + j - 1))neighbors ++;
        }
        score += neighbors * neighbors;
    }
    return score;
}
void sticky_algo::run(){
    auto put_a_stone = [&](stone_type stone){
        stone_type best_stone;
        int best_x,best_y;
        int best_score = std::numeric_limits<int>::min();
        for(int flip = 0; flip <= 1 ;flip++,stone.flip())for(int angle = 0; angle < 4;angle++,stone.rotate(90))for(int dy=-7;dy<=32;dy++)for(int dx=-7;dx<=32;dx++){
            int score = eval(problem.field,stone,dy,dx);
            if(best_score < score){
                best_score = score;
                best_y = dy;
                best_x = dx;
                best_stone = stone;
            }
        }
        if(best_score >= 0)problem.field.put_stone(best_stone,best_y,best_x);
        return;
    };
    int count = problem.stones.size();
    for(auto& _stone : problem.stones){
                put_a_stone(_stone);
                //std::ostringstream oss;
                //oss << (1-(double)count / (double)problem.stones.size()) *100 << "%完了";
                //print_text(oss.str());
                count --;
    }
    print_text("そうっすね");
    emit answer_ready(problem.field);
}
