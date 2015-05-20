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
    qRegisterMetaType<field_type>();
    problem = _problem;
    algo_vec.push_back(new simple_algorithm(problem));
    algo_vec.push_back(new poor_algo(problem));
    for(auto algo : algo_vec){
        connect(algo,&algorithm_type::answer_ready,this,&algorithm_manager::get_answer);
        connect(algo,&algorithm_type::finished,[=](){
            mtx.lock();
            boost::remove_erase(algo_vec,algo);
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
    if(best_zk < ans.get_score()){
        best_zk = ans.get_score();
        emit answer_ready(ans);
    }
    mtx.unlock();
}
