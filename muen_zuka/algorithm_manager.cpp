#include "algorithm_manager.hpp"
#include <QThread>
#include <QObject>
#include <vector>
#include <memory>
#include <boost/range/algorithm_ext/erase.hpp>
#include "tengu.hpp"
#include <iostream>
algorithm_manager::algorithm_manager(QObject *parent) : QObject(parent)
{
}
algorithm_manager::algorithm_manager(problem_type _problem)
{
    //Qtのsignal,slotで使いたい型の登録
    qRegisterMetaType<field_type>();
    qRegisterMetaType<std::string>();
    problem = _problem;
<<<<<<< HEAD
    //algo_vec.push_back(new simple_algorithm(problem));
    //algo_vec.push_back(new poor_algo(problem));
    algo_vec.push_back(new yrange(problem));
=======
    //動かしたいアルゴリズムを配列に入れる
    algo_vec.push_back(new simple_algorithm(problem));
    algo_vec.push_back(new sticky_algo(problem));
    algo_vec.push_back(new poor_algo(problem));
>>>>>>> develop
    for(auto algo : algo_vec){
        connect(algo,&algorithm_type::answer_ready,this,&algorithm_manager::get_answer);
        connect(algo,&algorithm_type::send_text,this,&algorithm_manager::get_text);
        connect(algo,&algorithm_type::finished,[=](){
            mtx.lock();
            boost::remove_erase(algo_vec,algo);
            if(algo_vec.size() == 0){
                emit finished();
            }
            mtx.unlock();
        });
    }
}
algorithm_manager::~algorithm_manager()
{

}
void algorithm_manager::run(){
    mtx.lock();
    for(algorithm_type* algo : algo_vec){
        algo->start();
    }
    mtx.unlock();
}
void algorithm_manager::get_answer(field_type ans){
    mtx.lock();
    //前より良ければemit
    if(best_zk > ans.get_score()){
        best_zk = ans.get_score();
        emit answer_ready(ans);
    }
    mtx.unlock();
}
int algorithm_manager::run_thread_num(){
    return algo_vec.size();
}
void algorithm_manager::get_text(std::string str){
    emit send_text(str);
}
