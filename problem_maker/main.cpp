#include <QCoreApplication>

#include "problem_maker.hpp"
#include "raw_stone.hpp"
#include <array>
#include <random>

#include <iostream>




int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    raw_stone stone;
    stone.create();

    //return a.exec();
    return 0;
}
