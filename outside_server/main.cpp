#include <QCoreApplication>
#include <QApplication>
#include <QSettings>
#include <QString>
#include <QFile>
#include <QDir>

#include <http/request_mapper.h>
#include <outside_server.h>

#include <global.h>
//GLOBAL VAR
bool g_stage_state_[48][48];
bool g_stone_state_[256][8][8];
int g_stone_num_;
std::vector<user_data_type> g_user_data;
bool g_user_data_updated=false;
bool g_need_rankingtag_updated=false;

int main(int argc, char *argv[])
{
    //mkdir
    QDir().mkdir("docroot");
    QDir().mkdir("docroot/problem");

    QApplication app(argc, argv);

    //Screen
    OutsideServer w;
    w.show();
    //Server
    RequestMapper request_mapper;

    return app.exec();
}
