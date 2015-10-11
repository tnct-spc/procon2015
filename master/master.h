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
#include <QSettings>
#include <mutex>

#include <boostconnect/server.hpp>
#include <boostconnect/session_type/http_session.hpp>

namespace Ui {
class Master;
}

class Master : public QWidget
{
    Q_OBJECT

public:
    typedef bstcon::server<bstcon::session::http_session> http_server;
    typedef boost::shared_ptr<bstcon::session::http_session> session_ptr;
    typedef bstcon::request request_type;

    explicit Master(QWidget *parent = 0);
    ~Master();

private:
    Ui::Master *ui;

    // Client
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

    // Server thread
    boost::shared_ptr<boost::asio::io_service> io_service_;
    boost::shared_ptr<http_server> server_;
    std::unique_ptr<std::thread> thread_;

private slots:
    // GUI
    void change_token_box();
    void reset_point();
    void select_send_1();
    void select_send_2();
    void select_send_3();
    void change_url_1();
    void change_url_2();
    void change_url_3();

    // HTTP Request Handler
    void Service(boost::shared_ptr<request_type> const req, session_ptr session);
    void ServiceRequestCompleted(QByteArray lowdata);
};

#endif // MASTER_H
