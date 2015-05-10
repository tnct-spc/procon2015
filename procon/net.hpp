#ifndef NET_H
#define NET_H

#include <string>
#include <iostream>
#include <stdexcept>

#include <QSharedPointer>
#include <QObject>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QByteArray>
#include <QEventLoop>
#include <QUrlQuery>

#include "takao.hpp"

//コンストラクタでURL指定してね
class net : public QObject
{
    Q_OBJECT
public:
    explicit net(QObject *parent = 0);
    net(QUrl server_url); //スタンドアロンで動作する
    net(QUrl server_url,QUrl master_url);
    net(QUrl server_url, QUrl master_url, std::string id, int problem_num);
    ~net();
    std::string get();
    std::string send(field_type answer);
signals:

public slots:

private slots:
    void networkerror(QNetworkReply::NetworkError);
private:
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    //std::shared_ptr<QNetworkAccessManager> manager;

    QUrl _server_url,_master_url;
    std::string _id;
    int _problem_num;
    bool network_error_flag =false;
};

#endif // NET_H
