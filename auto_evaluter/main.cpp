#include <QCoreApplication>
#include <algorithm_evaluater.hpp>
#include <QTimer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    algorithm_evaluater eval;
    QTimer::singleShot(0,&eval,SLOT(run()));
    return a.exec();
}
