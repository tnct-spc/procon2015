#include <QCoreApplication>
#include <algorithm_evaluater.hpp>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    algorithm_evaluater eval;
    auto q_vec = eval.load_problem_fires();
    eval.evaluate(q_vec.at(0));
}
