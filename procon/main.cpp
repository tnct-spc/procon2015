#include <iostream>
#include <QCoreApplication>
#include <takao.hpp>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Hello, world!" << std::endl;
    net  _net (QUrl("http://172.16.0.234:8080/files/problem1.txt"));
    std::cout << _net.get().constData() << std::endl;
    return a.exec();
}
