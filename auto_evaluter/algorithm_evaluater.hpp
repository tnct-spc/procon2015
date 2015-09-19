#ifndef ALGORITHM_EVALUATER_HPP
#define ALGORITHM_EVALUATER_HPP

#include <QObject>
#include <QDir>
#include <vector>
#include <QEventLoop>
#include <takao.hpp>
#include <tengu.hpp>
#include <iostream>
#include <QFile>
Q_DECLARE_METATYPE(field_type)
class algorithm_evaluater : public QObject
{
    Q_OBJECT
public:
    explicit algorithm_evaluater(QObject *parent = 0);
//private:
    std::vector<problem_type> load_problem_fires();
    void evaluate(problem_type);
    void save_answer(field_type);
    void save_record();
    algorithm_type* _algorithm;
    QStringList filelist;
    std::vector<problem_type> problem_vector;
    std::vector<field_type> answer_vector;
signals:

public slots:
    void get_answer(field_type ans);
};

#endif // ALGORITHM_EVALUATER_HPP
