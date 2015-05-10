#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <mutex>
#include <string>

#include <QObject>

#include "takao.hpp"

class processor : public QObject
{
   Q_OBJECT
public:
    processor(std::string raw_data);
    void run();
    ~processor();
private:
    problem_type  _problem;
    //std::vector<std::tuple<algorithm_type,bool>> v_algorithm;
    std::vector<bool> enable_algorithm;
    std::mutex mtx;
    int best_zk = 0;
private slots:
    void set_answer(field_type ans);
    void finished(algorithm_type* algo);
signals:
    void answer_return(field_type ans);
    void finished();
};

#endif // PROCESSOR_H
