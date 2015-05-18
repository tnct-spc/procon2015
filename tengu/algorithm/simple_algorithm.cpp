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
    for(auto stone : problem.stones){
        [&]{
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
        }();
    }
    emit answer_ready(problem.field);
}
