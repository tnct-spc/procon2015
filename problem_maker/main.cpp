#include <QCoreApplication>
#include <QTimer>
#include "problem_maker.hpp"

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
    problem_maker pm(argc, argv);
    QTimer::singleShot(0, &pm, SLOT(run()));

    return a.exec();
}
