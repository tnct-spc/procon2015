#include <iostream>
#include <QCoreApplication>
#include <takao.hpp>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Hello, world!" << std::endl;
    net  _net (QUrl("http://imoneg.eg"));
    std::cout << _net.get() << std::endl;
    return a.exec();
}
