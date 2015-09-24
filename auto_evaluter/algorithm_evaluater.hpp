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
    std::vector<problem_type> load_problem_fires();
    std::vector<field_type> evaluate(problem_type);
    void save_answer(std::tuple<std::string, field_type> named_answers);
    void save_problem(std::tuple<std::string, problem_type> named_problem);
    void save_record();
    std::tuple<std::string,problem_type> make_problem(std::string problem_name);
    algorithm_type* _algorithm;
    QStringList filelist;
    std::vector<problem_type> problem_vector;
    std::vector<field_type> answer_vector;
signals:

public slots:
    void get_answer(field_type ans);
    void run();
private:
    std::mutex mtx;
    const int loop_num = 100;
    QCoreApplication* app;
};

#endif // ALGORITHM_EVALUATER_HPP
