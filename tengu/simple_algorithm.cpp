#include "simple_algorithm.hpp"
#include <QFile>
#include <QIODevice>

simple_algorithm::simple_algorithm()
{

}

simple_algorithm::~simple_algorithm()
{

}

field_type simple_algorithm::run(std::string problem_data){

    problem_type problem(problem_data);


    //端から置ける場所を探して石を挿入していく
    for(unsigned int stone_num=0;stone_num<problem.stones.size();stone_num++){
        for(int dy=-8;dy<=32;dy++){
            for(int dx=-8;dx<=32;dx++){
                //フィールドに置けるかチェック
                if(problem.field.is_puttable(problem.stones[stone_num],dy,dx)){
                    //設置
                    problem.field.put_stone(problem.stones[stone_num],dy,dx);
                    goto NEXT_STONE;
                }
            }
        }
        NEXT_STONE:;
    }
    return problem.field;
}
