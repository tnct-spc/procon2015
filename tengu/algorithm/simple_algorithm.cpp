#include "simple_algorithm.hpp"
#include <QFile>
#include <QIODevice>
#include <iostream>
simple_algorithm::simple_algorithm(problem_type _problem)
{
    problem = _problem;
}

simple_algorithm::~simple_algorithm()
{

}

void simple_algorithm::run(){

    //problem_type problem(problem_data);

    auto put_func = [&](stone_type stone){
        for(int dy=-8;dy<=32;dy++){
            for(int dx=-8;dx<=32;dx++){
                for(int flip = 0; flip < 2; flip ++){
                     for(int angle = 0; angle < 4; angle ++){
                         if(problem.field.is_puttable(stone,dy,dx)){
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
    /*
    //端から置ける場所を探して石を挿入していく
    for(unsigned int stone_num=0;stone_num<problem.stones.size();stone_num++){
        for(int dy=-8;dy<=32;dy++){
            for(int dx=-8;dx<=32;dx++){
                //フィールドに置けるかチェック
                if(problem.field.is_puttable(problem.stones[stone_num],dy,dx)){
                    //設置
                    problem.field.put_stone(problem.stones[stone_num],dy,dx);
                    goto NEXT_STONE;
                    //お手軽GOTOを使うな
                }
            }
        }
        NEXT_STONE:;
    }
    */
    for(auto _stone : problem.stones){
                put_func(_stone);
    }

    for(auto stone : problem.field.list_of_stones()){
        std::cout << stone.get_angle() << std::endl;
    }
    //return problem.field;
    emit answer_ready(problem.field);
}
