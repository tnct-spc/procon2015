#include "outside_server.h"
#include <QApplication>

#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QString>
#include "httplistener.h"
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


    QString config_file_name=QCoreApplication::applicationDirPath()+"/etc/setting.ini";

    // Static file controller
    QSettings* file_settings=new QSettings(config_file_name,QSettings::IniFormat,&app);
    file_settings->beginGroup("files");
    RequestMapper::static_file_controller=new StaticFileController(file_settings,&app);

    // HTTP server
    QSettings* listener_settings=new QSettings(config_file_name,QSettings::IniFormat,&app);
    listener_settings->beginGroup("listener");
    new HttpListener(listener_settings,new RequestMapper(&app),&app);



    return app.exec();
}
