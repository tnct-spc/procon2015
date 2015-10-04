#include "algorithm_evaluater.hpp"
#include <tengu.hpp>
#include <QThread>
#include <QDebug>
#include <QVector>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentMap>
#include <QFuture>
#include <QIODevice>
#define LOAD_FROM_FILE
#define SAVE_ANSWER
#define RENAME_RECORD
algorithm_evaluater::algorithm_evaluater(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<field_type>();
}
std::vector<std::tuple<std::string,problem_type>> algorithm_evaluater::load_problem_fires(){
    std::vector<std::tuple<std::string,problem_type>> named_problem_vector;
    QDir q_dir("../../procon2015/problems");
    QStringList filelist = q_dir.entryList();
    for(auto filename : filelist){
        QFile file(QString("../../procon2015/problems/").append(filename));
        if(!file.open(QIODevice::ReadOnly))continue;
        qDebug() << file.fileName();
        QTextStream in(&file);
        named_problem_vector.push_back(std::make_tuple(file.fileName().toStdString(),problem_type(in.readAll().toStdString())));
    }
     return named_problem_vector;
}
std::vector<std::tuple<std::string,problem_type>> algorithm_evaluater::make_problem(){
    std::vector<std::tuple<std::string,problem_type>> named_problem_vcetor;
    for(int i = 0; i < loop_num; i++)
    {
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
        named_problem_vcetor.push_back(std::make_tuple(std::string("../../procon2015/problems/prob") + std::to_string(i) + ".txt",problem));
        save_problem(named_problem_vcetor.back());
    }
    return named_problem_vcetor;
}

void algorithm_evaluater::save_answer(std::tuple<std::string,field_type> named_answer){
    //問題のような名前の回答の名前
    QString answer_name_like_problem_name = std::get<0>(named_answer).c_str();
    int pos = answer_name_like_problem_name.indexOf("problems");
    answer_name_like_problem_name.replace(pos,13,"answers/ans");
    qDebug() << answer_name_like_problem_name;
    QFile file(answer_name_like_problem_name);
    if(!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    out << std::get<1>(named_answer).get_answer().c_str();
    file.close();
}
void algorithm_evaluater::save_problem(std::tuple<std::string, problem_type> named_problem){
    QFile file(QString(std::get<0>(named_problem).c_str()));
    if(!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    out << std::get<1>(named_problem).str().c_str();
    file.close();
}

field_type algorithm_evaluater::evaluate(problem_type problem,evaluator _eval){
    QEventLoop eventloop;
    field_type best_ans;
    int best_score = std::numeric_limits<int>::max();
    /********************************/
    /********************************/
    /********************************/
    /********************************/
    /********************************/
    //simple_algorithm algo(problem);
    sticky_algo algo(problem,_eval);
    /********************************/
    /********************************/
    /********************************/
    /********************************/
    /********************************/
    std::mutex mtx;
    algorithm_type::_best_score = std::numeric_limits<int>::max();
    connect(&algo,&algorithm_type::answer_ready,[&](field_type ans){
        mtx.lock();
        if(ans.get_score() < best_score){
            best_ans = ans;
            best_score = ans.get_score();
        }
        mtx.unlock();
        eventloop.quit();
    });
    connect(&algo,&algorithm_type::finished,&eventloop,&QEventLoop::quit);
    algo.start();
    eventloop.exec();
    algo.wait();
    return best_ans;
}
void algorithm_evaluater::run(){
    QDir ans_dir("../../procon2015/answers");
    QDir prob_dir("../../procon2015/problems");
    if(!ans_dir.exists())ans_dir.mkpath("../../procon2015/answers");
    if(!prob_dir.exists())prob_dir.mkpath("../../procon2015/problems");
    std::vector<std::tuple<std::string,problem_type>> named_problems;
    //named_problems = make_problem();
    named_problems = load_problem_fires();
    QVector<std::tuple<std::string,problem_type,double,double,double>> data;
    for(auto named_problem : named_problems){
        //qDebug() << std::get<0>(named_problem).c_str();
        for(double t_contact_pass = t_contact_pass_start; t_contact_pass <= t_contact_pass_end; t_contact_pass += t_contact_pass_step){
            for(double param_a = param_a_start; param_a <= param_a_end; param_a += param_a_step){
                for(double param_b = param_b_start; param_b <= param_b_end; param_b += param_b_step){
                    if(param_a + param_b > 1.0)break;
                    data.push_back(std::make_tuple(std::get<0>(named_problem),std::get<1>(named_problem),param_a,param_b,t_contact_pass));
                }
            }
        }
    }
    QFuture<void> threads = QtConcurrent::map(
        data,
        [this](auto tup){
            this->main_process(std::get<0>(tup),std::get<1>(tup),std::get<2>(tup),std::get<3>(tup),std::get<4>(tup));

        });
    qDebug() << "開始";
    threads.waitForFinished();
    QCoreApplication::exit(0);
}
void algorithm_evaluater::save_record(std::string prob_name, field_type answer, double param_a, double param_b, double t_pass){
    file_mtx.lock();
    QFile record_file("../../procon2015/recodes.txt");
    record_file.open(QIODevice::Append);
    QTextStream out(&record_file);
    //回答番号,スコア,石の数,param_a,param_b,t_contact_pass
    out << prob_name.c_str() << ","
        << answer.get_score() << ","
        << answer.get_stone_num() << ","
        << QString::number(param_a) << ","
        << QString::number(param_b) << ","
        << QString::number(t_pass)
        << endl;
    record_file.close();
    file_mtx.unlock();
}
void algorithm_evaluater::main_process(std::string prob_name, problem_type problem, double param_a, double param_b, double t_pass){
    evaluator _eval(param_a,param_b,t_pass);
    field_type answer = evaluate(problem,_eval);
    save_record(prob_name, answer, param_a, param_b, t_pass);
}

void algorithm_evaluater::get_answer(field_type ans){
}
