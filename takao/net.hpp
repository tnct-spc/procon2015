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
private:
    QNetworkAccessManager *manager;
    QUrl _url;
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
    eventloop.exec();
    return reply->readAll();
}
//デストラクタ書いてないからメモリリークするよ
//🍣　ご　め　ん　ね　🍣//
net::~net()
{

}

#endif // NET_H
