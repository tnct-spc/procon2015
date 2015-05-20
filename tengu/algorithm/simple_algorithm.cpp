#include "simple_algorithm.hpp"
#include <QFile>
#include <QIODevice>

simple_algorithm::simple_algorithm(problem_type _problem)
{
    problem = _problem;
}

simple_algorithm::~simple_algorithm()
{

}

void simple_algorithm::run(){

    //端から置ける場所を探して石を挿入していく関数
    auto put_a_stone = [&](stone_type stone){
        //設置可能範囲は左上座標(-7,-7)から左上座標(31,31)まで
        for(int dy=-7;dy<=32;dy++){
            for(int dx=-7;dx<=32;dx++){
                //反転
                for(int flip = 0; flip < 2; flip ++){
                    //回転
                     for(int angle = 0; angle < 4; angle ++){
                         //フィールドに置けるかチェック
                         if(problem.field.is_puttable(stone,dy,dx)){
                             //設置
                             problem.field.put_stone(stone,dy,dx);
                             return;
                         }
                         stone.rotate(90);
                      }
                  stone.flip();
                }
            }
        }
    };

    //石を設置
    for(auto _stone : problem.stones){
                put_a_stone(_stone);
    }

    //解答の送信
    std::cout << "simple" << std::endl;
    emit answer_ready(problem.field);
}
