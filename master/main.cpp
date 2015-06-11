#include <QCoreApplication>
#include <QApplication>
#include <QSettings>
#include <QString>
#include <QFile>
#include <QDir>

#include "server.h"
#include "master.h"

#include <global.h>
//GLOBAL VAR
QString g_token_name="名無し";//default name
std::vector<answer_data_type> g_answer_data;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //Screen
    Master w;
    w.show();
    //Server
    Server server;

    return app.exec();
}
