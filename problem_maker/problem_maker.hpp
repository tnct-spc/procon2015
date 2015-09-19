#ifndef PROBLEM_MAKER_HPP
#define PROBLEM_MAKER_HPP

#include <QObject>
#include <random>
#include <field_type.hpp>
#include <tengu.hpp>

Q_DECLARE_METATYPE(field_type)
Q_DECLARE_METATYPE(std::string)

class problem_maker : public QObject
{
    Q_OBJECT
public:
    problem_maker(int argc, char **argv);
public slots:
    void run();
private:
    using test_algo = poor_algo;
    bool error = false;
    int problems = 0;
    int rows, cols;
    int obstacles;
    int create_answer;
    int min_zk, max_zk; // per stone
    std::string ansfilename;
    field_type ans_field;
};

#endif // PROBLEM_MAKER_HPP

