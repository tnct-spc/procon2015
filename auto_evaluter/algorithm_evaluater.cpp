#include "algorithm_evaluater.hpp"
#include <tengu.hpp>
#include <QThread>
algorithm_evaluater::algorithm_evaluater(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<field_type>();
}
std::vector<problem_type> algorithm_evaluater::load_problem_fires(){
    std::vector<problem_type> problem_vector;
    QDir q_dir("../problems");
    if(!q_dir.exists()){
        q_dir.mkpath("../problems");
        std::cout << "ディレクトリがねぇよ" << std::endl;
        return problem_vector;
    }
    filelist = q_dir.entryList();
    QStringList::const_iterator f_itr;
    //qDebug() << filelist;
    for (f_itr = filelist.constBegin(); f_itr != filelist.constEnd(); ++f_itr){
        QFile file(QString("../problems/").append(*f_itr));
        //qDebug() << file.fileName();
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
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());

    std::uniform_int_distribution<int> dist_size(1, FIELD_SIZE);
    rows = dist_size(engine);
    cols = dist_size(engine);
    std::uniform_int_distribution<int> dist_obs(0, std::min(50, rows * cols));
    obstacles = dist_obs(engine);
    min_zk = 1;
    max_zk = 16;

    field_type field(obstacles, cols, rows);
    //field.set_random(obstacles, cols, rows);
    std::vector<stone_type> stones;
    int stone_num = 1; //初期値は1
    int const minimum_zk = field.empty_zk();
    std::cout << "empty blocks: " << minimum_zk << std::endl;
    for(int total_zk = 0; total_zk < minimum_zk; ) {
        std::uniform_int_distribution<int> dist_zk(min_zk, max_zk);
        int const zk = dist_zk(engine);
        stone_type stone(zk,stone_num);
        qDebug() << stone.str().c_str();
        stone_num ++;
        total_zk += stone.get_area();
        stones.push_back(stone);
    }
    field.set_provided_stones(stones.size());
    problem_type problem(field, stones);
    return std::make_tuple(problem_name,problem);
}

void algorithm_evaluater::save_answer(std::tuple<std::string,field_type> named_answer){
    QFile file(QString(std::get<0>(named_answer).c_str()).append("ans.txt"));
    if(!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    out << std::get<1>(named_answer).get_answer().c_str();
}
void algorithm_evaluater::save_problem(std::tuple<std::string, problem_type> named_problem){
    QFile file(QString(std::get<0>(named_problem).c_str()).append("prob.txt"));
    if(!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    out << std::get<1>(named_problem).str().c_str();
}

std::vector<field_type> algorithm_evaluater::evaluate(problem_type problem){
    QEventLoop eventloop;
    std::vector<field_type> ans_vector;
    simple_algorithm algo(problem);
    algorithm_type::_best_score = std::numeric_limits<int>::max();
    connect(&algo,&algorithm_type::answer_ready,[&](field_type ans){
        mtx.lock();
        ans_vector.push_back(ans);
        qDebug() << "hello";
        mtx.unlock();
        eventloop.quit();
    });
    connect(&algo,&algorithm_type::finished,&eventloop,&QEventLoop::quit);
    algo.start();
    eventloop.exec();
    qDebug() << "ohayo";
    algo.wait();
    return ans_vector;
}
void algorithm_evaluater::run(){
    std::vector<std::tuple<std::string,field_type>> named_answers;
    for(int prob_num = 0; prob_num < loop_num;prob_num++){
        qDebug() << "a" << prob_num;
        auto named_problem = make_problem(std::to_string(prob_num));
        save_problem(named_problem);
        auto answers = evaluate(std::get<1>(named_problem));
        for(int ans_num = 0; ans_num < answers.size();ans_num++){
            auto named_answer = std::make_tuple((std::get<0>(named_problem)) += std::string("-") += std::to_string(ans_num),answers.at(ans_num));
            named_answers.push_back(named_answer);
            save_answer(named_answer);
        }
    }
    QCoreApplication::exit(0);
}

void algorithm_evaluater::save_record(){

}
void algorithm_evaluater::get_answer(field_type ans){
}
