#include "algorithm_evaluater.hpp"
#include <tengu.hpp>
#include <QThread>
#include <QDebug>
#define LOAD_TO_FILE
algorithm_evaluater::algorithm_evaluater(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<field_type>();
}
std::vector<problem_type> algorithm_evaluater::load_problem_fires(){
    std::vector<problem_type> problem_vector;
    QDir q_dir("../../procon2015/problems");
    filelist = q_dir.entryList();
    for(auto filename : filelist){
        QFile file(QString("../../procon2015/problems/").append(filename));
        if(!file.open(QIODevice::ReadOnly))continue;
        qDebug() << file.fileName();
        QTextStream in(&file);
        problem_vector.push_back(problem_type(in.readAll().toStdString()));
    }
     return problem_vector;
}
std::tuple<std::string,problem_type> algorithm_evaluater::make_problem(std::string problem_name){
    int rows, cols;
    int obstacles;
    int min_zk, max_zk; // per stone
    int max_stone_num;
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());

    std::uniform_int_distribution<int> dist_size(20, FIELD_SIZE);
    rows = dist_size(engine);
    cols = dist_size(engine);
    std::uniform_int_distribution<int> dist_obs(0, std::min(50, rows * cols -1));
    obstacles = dist_obs(engine);
    min_zk = 1;
    max_zk = 16;
    std::uniform_int_distribution<int> dist_stone_num(1,256);
    max_stone_num = dist_stone_num(engine);
    field_type field(obstacles, cols, rows);
    std::vector<stone_type> stones;
    for(int stone_num = 1; stone_num < max_stone_num; stone_num ++) {
        std::uniform_int_distribution<int> dist_zk(min_zk, max_zk);
        int const zk = dist_zk(engine);
        stone_type stone(zk,stone_num);
        stones.push_back(stone);
    }
    field.set_provided_stones(stones.size());
    problem_type problem(field, stones);
    return std::make_tuple(problem_name,problem);
}

void algorithm_evaluater::save_answer(std::tuple<std::string,field_type> named_answer){
    QFile file(QString("../../procon2015/answers/") += QString(std::get<0>(named_answer).c_str()).append("ans.txt"));
    if(!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    out << std::get<1>(named_answer).get_answer().c_str();
    file.close();
}
void algorithm_evaluater::save_problem(std::tuple<std::string, problem_type> named_problem){
    QFile file(QString("../../procon2015/problems/") += QString(std::get<0>(named_problem).c_str()).append("prob.txt"));
    if(!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    out << std::get<1>(named_problem).str().c_str();
    file.close();
}

std::vector<field_type> algorithm_evaluater::evaluate(problem_type problem){
    QEventLoop eventloop;
    std::vector<field_type> ans_vector;
    /********************************/
    /********************************/
    /********************************/
    /********************************/
    /********************************/
    //simple_algorithm algo(problem);
    sticky_algo algo(problem);
    /********************************/
    /********************************/
    /********************************/
    /********************************/
    /********************************/
    algorithm_type::_best_score = std::numeric_limits<int>::max();
    connect(&algo,&algorithm_type::answer_ready,[&](field_type ans){
        mtx.lock();
        ans_vector.push_back(ans);
        mtx.unlock();
        eventloop.quit();
    });
    connect(&algo,&algorithm_type::finished,&eventloop,&QEventLoop::quit);
    algo.start();
    eventloop.exec();
    algo.wait();
    return ans_vector;
}
void algorithm_evaluater::run(){
    std::vector<std::tuple<std::string,field_type>> named_answers;
    QDir ans_dir("../../procon2015/answers");
    QDir prob_dir("../../procon2015/problems");
    if(!ans_dir.exists())ans_dir.mkpath("../../procon2015/answers");
    if(!prob_dir.exists())prob_dir.mkpath("../../procon2015/problems");

#ifdef LOAD_TO_FILE
    std::vector<std::tuple<std::string,problem_type>> named_problems;
    auto prob_vec = load_problem_fires();
    int i = 0;
    for(auto prob : prob_vec){
        named_problems.push_back(make_tuple(std::string("q") += std::to_string(i),prob));
        i++;
    }
    loop_num = i;
#endif
    for(int prob_num = 0; prob_num < loop_num;prob_num++){
        qDebug() << "problem_number:" << prob_num;
#ifdef LOAD_TO_FILE
        auto named_problem = named_problems.at(prob_num);
#else
        auto named_problem = make_problem(std::to_string(prob_num));
#endif
        save_problem(named_problem);
        auto answers = evaluate(std::get<1>(named_problem));
        auto best_ans = std::min_element(answers.begin(),answers.end(),[](auto const &t1, auto const &t2){return t1.get_score() < t2.get_score();});
        auto named_answer = std::make_tuple((std::get<0>(named_problem)),*best_ans);
        named_answers.push_back(named_answer);
        save_answer(named_answer);
        save_record(named_problem,named_answer);
    }
    QCoreApplication::exit(0);
}

void algorithm_evaluater::save_record(std::tuple<std::string, problem_type> named_problem, std::tuple<std::string, field_type> named_answer){
    QFile record_file("../../procon2015/recodes.txt");
    record_file.open(QIODevice::Append);
    QTextStream out(&record_file);
    //回答番号,問題番号,スコア,石の数
    out << std::get<0>(named_answer).c_str() << "," << std::get<0>(named_problem).c_str() << "," << std::get<1>(named_answer).get_score() << "," << std::get<1>(named_answer).get_stone_num() << endl;
}

void algorithm_evaluater::get_answer(field_type ans){
}
