#include "algorithm_type.hpp"
#include "QDebug"
/*
algorithm_type::algorithm_type(problem_type const& _problem): problem(_problem)
{
}
*/
std::mutex algorithm_type::ans_emit_mtx;

void algorithm_type::print_text(std::string str){
    emit send_text("<" + algorithm_name + ">" + str);
}
void algorithm_type::answer_send(field_type ans){
    ans_emit_mtx.lock();
    emit answer_ready(ans);
    //5ms待つ
    QThread::msleep(5);
    ans_emit_mtx.unlock();
}
