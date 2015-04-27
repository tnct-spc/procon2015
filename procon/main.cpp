#include <iostream>
#include <QCoreApplication>
#include <data_type.hpp>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Hello, world!" << std::endl;

    return a.exec();
}
