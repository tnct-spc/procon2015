#include "slave.h"
#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Slave w;
    w.show();
    if(argc == 3) {
        w.force_enable_algorithm(QString(argv[1]).toInt());
        w.force_start(argv[2]);
    }
    a.processEvents();
    return a.exec();
}
