#include "processor.hpp"
#include<string>
#include<vector>
#include<mutex>
#include<queue>
#include <QObject>

processor::processor(std::string raw_data){
    _problem = problem_type(raw_data);
}
void processor::run(){
    /*
    for(auto algo : v_algorithm){
        if(std::get<1>(algo)){
            thread_grp.create_thread(std::get<0>(algo).run());
        }
    }
    */
}
void processor::set_answer(field_type ans){
    /*
     * 本来は下記の実装
     */
    /*
    mtx.lock();
    if(best_zk < ans.zk()){
        best_zk = ans.zk();
        emit answer_return(ans);
    }
    mtx.unlock();
    */
}
void processor::finished(algorithm_type* algo){
    /*
    delete algo;
    if(thread_grp.size() ==0){
        emit finished();
    }
    */
}
processor::~processor(){

}
