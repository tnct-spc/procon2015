#ifndef SERVER_H
#define SERVER_H

#include <QCoreApplication>
#include <QObject>
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
#include <src/qhttpserver.h>
#include <src/qhttprequest.h>
#include <src/qhttpresponse.h>
#include <global.h>


class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);

private slots:
    void Service(QHttpRequest *request, QHttpResponse *response);
    void ServiceRequestCompleted(QByteArray lowdata);

private:
    QHttpResponse *new_response_;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString _master_url="http://172.16.0.66:8080/answer";
    //QString _master_url="http://localhost:8080/answer";
};

#endif // SERVER_H
