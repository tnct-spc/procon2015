#include "outside_server.h"
#include <QApplication>

#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QString>
#include <http/request_mapper.h>

#include <global.h>
//GLOBAL VAR
bool g_stage_state_[48][48];
bool g_stone_state_[256][8][8];
int g_stone_num_;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    OutsideServer w;
    w.showFullScreen();


    RequestMapper request_mapper;


    return app.exec();
}
