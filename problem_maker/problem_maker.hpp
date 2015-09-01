#ifndef PROBLEM_MAKER_HPP
#define PROBLEM_MAKER_HPP

#include <QObject>
#include <random>

class problem_maker : public QObject
{
    Q_OBJECT
public:
    problem_maker(int argc, char **argv);
public slots:
    void run();
private:
    bool error = false;
    int problems = 0;
    int rows, cols;
    int obstacles;
    int create_answer;
    int min_zk, max_zk; // per stone
};

#endif // PROBLEM_MAKER_HPP

