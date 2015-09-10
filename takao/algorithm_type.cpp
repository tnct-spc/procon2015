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
    if(ans.get_score() < _best_score){
        _best_score = ans.get_score();
        emit answer_ready(ans);
        //50ms待つ
        QThread::msleep(50);
    }
    ans_emit_mtx.unlock();
}
