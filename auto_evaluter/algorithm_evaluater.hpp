#ifndef ALGORITHM_EVALUATER_HPP
#define ALGORITHM_EVALUATER_HPP

#include <QObject>
#include <QDir>
#include <vector>
#include <QEventLoop>
#include <takao.hpp>
#include <tengu.hpp>
#include <iostream>
#include <mutex>
#include <tuple>
#include <string>
#include <QCoreApplication>
#include <QFile>
#include <random>
#include <iostream>
#include <limits>
Q_DECLARE_METATYPE(field_type)
class algorithm_evaluater : public QObject
{
    Q_OBJECT
public:
    explicit algorithm_evaluater(QObject *parent = 0);
//private:
    std::vector<std::tuple<std::string, problem_type> > load_problem_fires();
    std::vector<field_type> evaluate(problem_type, evaluator _eval);
    void save_answer(std::tuple<std::string, field_type> named_answer);
    void save_problem(std::tuple<std::string, problem_type> named_problem);
    void save_record(std::tuple<std::string, problem_type> named_problem, std::tuple<std::string, field_type> named_answer,std::tuple<double,double,double,double> params);
    std::vector<std::tuple<std::string, problem_type> > make_problem();
    algorithm_type* _algorithm;
    QStringList filelist;
    std::vector<problem_type> problem_vector;
    std::vector<field_type> answer_vector;
signals:

public slots:
    void get_answer(field_type ans);
    void run();
private:
    void main_process(std::tuple<std::string,problem_type> named_problem, evaluator _eval, std::tuple<double, double, double, double> params);
    std::mutex mtx;
    int loop_num = 100;
    QCoreApplication* app;
    double static constexpr w_complexity_start = 0;
    double static constexpr w_complexity_step = 10;
    double static constexpr w_complexity_end = 1;

    double static constexpr w_contact_move_start = 0;
    double static constexpr w_contact_move_step = 10;
    double static constexpr w_contact_move_end = 1;

    double static constexpr w_nextbranches_start = 0;
    double static constexpr w_nextbranches_step = 10;
    double static constexpr w_nextbranches_end = 1;

    double static constexpr w_contact_pass_start = 0;
    double static constexpr w_contact_pass_step = 10;
    double static constexpr w_contact_pass_end = 1;
};

#endif // ALGORITHM_EVALUATER_HPP
