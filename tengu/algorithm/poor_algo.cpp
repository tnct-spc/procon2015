#include "poor_algo.hpp"
#include <iostream>
poor_algo::poor_algo(problem_type _problem)
{
    problem = _problem;
    algorithm_name = "poor";
}

poor_algo::~poor_algo()
{

}

void poor_algo::run(){
    //端から置ける場所を探して石を挿入していく関数
    auto put_a_stone = [&](stone_type stone){
        //設置可能範囲は左上座標(-7,-7)から左上座標(31,31)まで
        for(int dy=-7;dy<=32;dy++){
            for(int dx=-7;dx<=32;dx++){
                //フィールドに置けるかチェック
                if(problem.field.is_puttable(stone,dy,dx)){
                    //設置
                    problem.field.put_stone(stone,dy,dx);
                    return;
                }
            }
        }
    };

    //石を設置
    for(auto _stone : problem.stones){
                put_a_stone(_stone);
    }

    //解答の送信
    answer_send(problem.field);
}
