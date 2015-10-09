#include "algorithm_manager.hpp"
#include <QThread>
#include <QObject>
#include <vector>
#include <memory>
#include <boost/range/algorithm_ext/erase.hpp>
#include "tengu.hpp"
#include <iostream>
#include <limits>
#include "evaluator.hpp"
algorithm_manager::algorithm_manager(QObject *parent) : QObject(parent)
{
}
algorithm_manager::algorithm_manager(problem_type _problem,std::vector<bool> enable_algo, int time_limit)
{
    //Qtのsignal,slotで使いたい型の登録
    qRegisterMetaType<field_type>();
    qRegisterMetaType<std::string>();
    problem = _problem;
    enable_algorithm_list = enable_algo;
    //アルゴリズムの最適解値を初期化
    algorithm_type::_best_score = std::numeric_limits<int>::max();
    // 評価関数をインスタンス化
    evaluator eval(-10, 1, 1, 0.5);
    //動かしたいアルゴリズムを配列に入れる
    if(enable_algo.at(0))algo_vec.push_back(new simple_algorithm(problem));
    if(enable_algo.at(1))algo_vec.push_back(new poor_algo(problem));
    if(enable_algo.at(2))algo_vec.push_back(new sticky_algo(problem));
    //if(enable_algo.at(3))algo_vec.push_back(new square(problem));
    if(enable_algo.at(4))algo_vec.push_back(new yrange(problem, time_limit,eval));
    //if(enable_algo.at(5))algo_vec.push_back(new yrange2(problem));
    if(enable_algo.at(6))algo_vec.push_back(new read_ahead(problem, eval));
    if(enable_algo.at(7))algo_vec.push_back(new new_beam(problem, eval));
    if(enable_algo.at(8))algo_vec.push_back(new sticky_beam(problem,evaluator(-10,1,1,0.5)));
    if(enable_algo.at(9))algo_vec.push_back(new yrange_based_yayoi(problem,time_limit,eval));


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
    if(enable_algorithm_list.at(5))emit send_text("yrange2動作");
    if(enable_algorithm_list.at(6))emit send_text("read_ahead動作");
    if(enable_algorithm_list.at(7))emit send_text("new_beam動作");
    if(enable_algorithm_list.at(8))emit send_text("stiky_beam動作");
    if(enable_algorithm_list.at(9))emit send_text("yrange_based_yayoi動作");
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
