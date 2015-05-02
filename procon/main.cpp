#include <iostream>
#include <QCoreApplication>
#include <takao.hpp>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Hello, world!" << std::endl;
    net  _net (QUrl("http://127.0.0.1:3000/post.rb"));
    std::cout << _net.get() << std::endl;
    _net.send();
    return a.exec();
}
