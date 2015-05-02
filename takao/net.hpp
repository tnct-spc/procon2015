#ifndef NET_H
#define NET_H

#include <QObject>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QByteArray>
#include <QEventLoop>
#include<iostream>
#include <stdexcept>
//コンストラクタでURL指定してね
class net : public QObject
{
    Q_OBJECT
public:
    explicit net(QObject *parent = 0);
    net(QUrl url);
    ~net();
    QByteArray get();
signals:

public slots:

private slots:
    void networkerror(QNetworkReply::NetworkError);
private:
    QNetworkAccessManager *manager;
    QUrl _url;
};
class Network_Timeout_Exception : public std::runtime_error{
public:
    Network_Timeout_Exception(const std::string& cause) : std::runtime_error("cause: " + cause ){}
};

net::net(QUrl url)
{
    _url = url;
}
//GETで取ってくる
QByteArray net::get()
{
    manager = new QNetworkAccessManager(this);
    QEventLoop eventloop;

    connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
    QNetworkReply *reply = manager->get(QNetworkRequest(_url));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    try{
    eventloop.exec();
    } catch (const Network_Timeout_Exception& e){
        std::cout << e.what() << std::endl;
    }
    return reply->readAll();
}
//デストラクタ書いてないからメモリリークするよ
//🍣　ご　め　ん　ね　🍣//
net::~net()
{
    delete manager;

}
void net::networkerror(QNetworkReply::NetworkError e){
    throw Network_Timeout_Exception("unreachable host");
}

#endif // NET_H
