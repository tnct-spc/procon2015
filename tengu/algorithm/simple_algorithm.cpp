#include "simple_algorithm.hpp"
#include <QFile>
#include <QIODevice>

simple_algorithm::simple_algorithm(problem_type _problem)
{
    pre_problem = _problem;
    algorithm_name = "simple";
}

simple_algorithm::~simple_algorithm()
{

}

void simple_algorithm::run(){
    int temp;
    //端から置ける場所を探して石を挿入していく関数
    auto put_a_stone = [&](stone_type stone, int mode){
        //設置可能範囲は左上座標(-7,-7)から左上座標(31,31)まで
        for(int dy=-7;dy<=32;dy++){
            for(int dx=-7;dx<=32;dx++){

                //8beam
                if(mode%4 >= 2) dy=25-dy;//上下反転
                if(mode%2 == 1) dx=25-dx;//左右反転
                if(mode   >= 4){//向き回転
                    temp=dy;
                    dy=dx;
                    dx=temp;
                }

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

                //8beamを元に戻す
                if(mode   >= 4){
                    temp=dy;
                    dy=dx;
                    dx=temp;
                }
                if(mode%2 == 1) dx=25-dx;
                if(mode%4 >= 2) dy=25-dy;

            }
        }
    };

    for(int i=0;i<8;i++){
        problem=pre_problem;
        //石を設置
        for(auto _stone : problem.stones){
            put_a_stone(_stone,i);
        }
        //解答の送信
        qDebug("emit %d",i);
        emit answer_ready(problem.field);
    }
}
