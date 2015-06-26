#include <QCoreApplication>
#include <QApplication>
#include <QSettings>
#include <QString>
#include <QFile>
#include <QDir>

#include "master.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Master w;
    w.show();

    return app.exec();
}
