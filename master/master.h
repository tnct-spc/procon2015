#ifndef MASTER_H
#define MASTER_H

#include <QCoreApplication>
#include <QObject>
#include <QWidget>
#include <QFile>
#include <QUrlQuery>
#include <QRegExp>
#include <QObject>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QByteArray>
#include <QEventLoop>
#include <QUrlQuery>
#include <QSharedPointer>
#include "net.hpp"
#include "takao.hpp"
#include "muen_zuka.hpp"
#include "src/qhttpserver.h"
#include "src/qhttprequest.h"
#include "src/qhttpresponse.h"
#include <QSettings>
#include <mutex>

namespace Ui {
class Master;
}

class Master : public QWidget
{
    Q_OBJECT

public:
    explicit Master(QWidget *parent = 0);
    ~Master();

private:
    Ui::Master *ui;
    QHttpResponse *new_response_;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    struct answer_data_type{
        QString answer_raw_data;
        int answer_point;
        int answer_processes_size;
        QString problem_number;
    };

    QString token_name_;
    std::vector<answer_data_type> answer_data_;

    QString get_sendurl();
    QSettings* settings;

    std::mutex network_mutex;

private slots:
    void change_token_box();
    void reset_point();
    void select_send_1();
    void select_send_2();
    void select_send_3();
    void change_url_1();
    void change_url_2();
    void change_url_3();
    void Service(QHttpRequest *request, QHttpResponse *response);
    void ServiceRequestCompleted(QByteArray lowdata);
};

#endif // MASTER_H
