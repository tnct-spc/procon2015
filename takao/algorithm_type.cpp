#include "algorithm_type.hpp"
#include "QDebug"
#include <limits>
/*
algorithm_type::algorithm_type(problem_type const& _problem): problem(_problem)
{
}
*/
std::mutex algorithm_type::ans_emit_mtx;
int algorithm_type::_best_score = std::numeric_limits<int>::max();
void algorithm_type::print_text(std::string str){
    emit send_text("<" + algorithm_name + ">" + str);
}
void algorithm_type::answer_send(field_type ans){
    ans_emit_mtx.lock();
    if(ans.get_score() < static_cast<size_t>(_best_score)){
        _best_score = ans.get_score();
        _best_processes_num = ans.processes.size();
        emit answer_ready(ans);
        //50ms待つ
        QThread::msleep(50);
    }else if(ans.get_score() == static_cast<size_t>(_best_score)){
        if(ans.processes.size() < static_cast<size_t>(_best_processes_num)){
            _best_score = ans.get_score();
            _best_processes_num = ans.processes.size();
            emit answer_ready(ans);
            //50ms待つ
            QThread::msleep(50);
        }
    }
    ans_emit_mtx.unlock();
}

int algorithm_type::get_rem_stone_zk(stone_type& stone)
{
    int sum = 0;
    for(std::size_t i = stone.get_nth() - 1; i < problem.stones.size(); ++i)
    {
        sum += problem.stones.at(i).get_area();
    }
    return sum;
}

int algorithm_type::get_rem_stone_zk(int stone_num)
{
    int sum = 0;
    for(std::size_t i = stone_num; i < problem.stones.size(); ++i)
    {
        sum += problem.stones.at(i).get_area();
    }
    return sum;
}
