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
algorithm_manager::algorithm_manager(problem_type _problem,std::vector<bool> enable_algo)
{
    //Qtのsignal,slotで使いたい型の登録
    qRegisterMetaType<field_type>();
    qRegisterMetaType<std::string>();
    problem = _problem;
    enable_algorithm_list = enable_algo;
    //動かしたいアルゴリズムを配列に入れる
    if(enable_algo.at(0))algo_vec.push_back(new simple_algorithm(problem));
    if(enable_algo.at(1))algo_vec.push_back(new poor_algo(problem));
    if(enable_algo.at(2))algo_vec.push_back(new sticky_algo(problem));
    if(enable_algo.at(3))algo_vec.push_back(new square(problem));
    if(enable_algo.at(4))algo_vec.push_back(new yrange(problem));

    for(auto algo : algo_vec){
        algo->setParent(this);
        connect(algo,&algorithm_type::answer_ready,this,&algorithm_manager::get_answer);
        connect(algo,&algorithm_type::send_text,this,&algorithm_manager::get_text);

        connect(algo,&algorithm_type::finished,[=](){
            mtx.lock();
            boost::remove_erase(algo_vec,algo);
            if(algo_vec.size() == 0)emit finished();
            //delete algo;
            mtx.unlock();
        });

        connect(algo,&algorithm_type::finished,algo,&algorithm_type::deleteLater);
    }
}
algorithm_manager::~algorithm_manager()
{

}
void algorithm_manager::run(){
    if(enable_algorithm_list.at(0))emit send_text("simple動作");
    if(enable_algorithm_list.at(1))emit send_text("poor動作");
    if(enable_algorithm_list.at(2))emit send_text("sticky動作");
    if(enable_algorithm_list.at(3))emit send_text("square動作");
    if(enable_algorithm_list.at(4))emit send_text("yrange動作");
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
