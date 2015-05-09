#ifndef PROCESSOR_H
#define PROCESSOR_H
#include<QObject>

#include<string>
#include<vector>
#include<mutex>
#include<boost/optional.hpp>
#include<boost/thread.hpp>
#include<queue>
#include "takao.hpp"

//アルゴリズムの名前の型列挙
/*
enum Algorithm_name{
    ALGO1,Y_RANGE_2,MURAKAMI_DX
};
*/
class SHARED_EXPORT processor : public QObject
{
    Q_OBJECT
public:
    explicit processor(QObject *parent = 0);
    ~processor();

    //boost::optional<field_type> get();
private:
    //problem_type _problem;
    //std::vector<std::tuple<algorithm_type,bool>> algo_v;
    //boost::thread_group _thread_group;
    //std::mutex mtx;
    //int best_zk = 0;
signals:
public slots:
    //void algo_set_enable(Algorithm_name,bool);
private slots:
    //void set_ans(field_type ans);
    //void algo_finished(algorithm_type* algo);
};

processor::processor(QObject *parent){
    //int a=0;
    //a++;
}

#endif // PROCESSOR_H
