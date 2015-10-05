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
    field_type evaluate(problem_type, evaluator _eval);
    void save_answer(std::tuple<std::string, field_type> named_answer);
    void save_answer(std::string ans_name, field_type ans);
    void save_problem(std::tuple<std::string, problem_type> named_problem);
    void save_record(std::string prob_name, field_type answer, double param_a, double param_b, double t_pass);
    std::vector<std::tuple<std::string, problem_type> > make_problem();
signals:

public slots:
    void get_answer(field_type ans);
    void run();
private:
    void main_process(std::string prob_name, problem_type problem, double param_a, double param_b, double t_pass);
    int loop_num = 100;
    std::mutex file_mtx;
    //t_contact_passのとりうる範囲は0~1です
    double static constexpr t_contact_pass_start = 0.0;
    double static constexpr t_contact_pass_step = 0.1;
    double static constexpr t_contact_pass_end = 1.0;
    //param_aのとりうる範囲は0~1です
    //param_a + param_b <= 1 にしてください
    double static constexpr param_a_start = 0.0;
    double static constexpr param_a_step = 0.1;
    double static constexpr param_a_end = 1.0;
    //param_bのとりうる範囲は0~1です
    //param_a + param_b <= 1 にしてください
    double static constexpr param_b_start = 0.0;
    double static constexpr param_b_step = 0.1;
    double static constexpr param_b_end = 1.0;
};

#endif // ALGORITHM_EVALUATER_HPP
