#include <iostream>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Hello, world!" << std::endl;

    return a.exec();
}
