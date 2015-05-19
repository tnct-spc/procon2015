#ifndef ALGORITHM_MANAGER_H
#define ALGORITHM_MANAGER_H

#include <QObject>
#include <string>
#include <mutex>
#include "takao.hpp"
#include "tengu.hpp"
Q_DECLARE_METATYPE(field_type)
class algorithm_manager : public QObject
{
    Q_OBJECT
public:
    explicit algorithm_manager(QObject *parent = 0);
    algorithm_manager(problem_type _problem);
    ~algorithm_manager();

    void run();

private:
    std::vector<algorithm_type*> algo_vec;
    std::mutex mtx;
    problem_type problem;
    u_int64_t best_zk = 0;

signals:
    void answer_ready(field_type ans);

public slots:
    void get_answer(field_type ans);
};

#endif // ALGORITHM_MANAGER_H
