#ifndef NET_H
#define NET_H

#include <QObject>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QByteArray>
#include <QEventLoop>
#include <QUrlQuery>
#include<iostream>
#include <stdexcept>
#include <QSharedPointer>
#include "takao.hpp"
//コンストラクタでURL指定してね
class SHARED_EXPORT net : public QObject
{
    Q_OBJECT
public:
    explicit net(QObject *parent = 0);
    net(QUrl server_url); //スタンドアロンで動作する
    net(QUrl server_url,QUrl master_url);
    ~net();
    std::string get();
    void send(answer_type answer);
    void send();
signals:

public slots:

private slots:
    void networkerror(QNetworkReply::NetworkError);
private:
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    //std::shared_ptr<QNetworkAccessManager> manager;

    QUrl _server_url,_master_url;
    bool network_error_flag =false;
};
class Network_Timeout_Exception : public std::runtime_error{
public:
    Network_Timeout_Exception(const std::string& cause) : std::runtime_error("cause: " + cause ){}
};

net::net(QUrl server_url)
{
    _server_url = server_url;
    _master_url = server_url;
}
net::net(QUrl server_url,QUrl master_url)
{
    _server_url = server_url;
    _master_url = master_url;
}
//GETで取ってくる
std::string net::get()
{
    //manager = new QNetworkAccessManager(this);
    QEventLoop eventloop;

    connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
    QNetworkReply *reply = manager->get(QNetworkRequest(_server_url));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    eventloop.exec();
    manager->disconnect();
    if(network_error_flag)return std::string("");
    return std::string(reply->readAll().constData());
}
//sendはまだテストしてない何が起こるかわからない
void net::send(answer_type answer){
    QEventLoop eventloop;
    QByteArray raw_data(answer.get_answer_str().c_str(),-1);
    QByteArray postData;
    connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
   // QNetworkReply *reply = manager->post(QNetworkRequest(_master_url),raw_data);
    QNetworkReply *reply = manager->post(QNetworkRequest(_master_url),postData);
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    eventloop.exec();
}
void net::send(){
    std::cout << "in send ()" << std::endl;
    QEventLoop eventloop;
    QUrlQuery postData;
    postData.addQueryItem("tweet","Qt post test");
    QNetworkRequest req(_master_url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
    QNetworkReply *reply = manager->post(req,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    eventloop.exec();
}

//デストラクタ書いてないからメモリリークするよ
//🍣　ご　め　ん　ね　🍣//
//追記 多分もうしない
net::~net()
{
    delete manager;

}
void net::networkerror(QNetworkReply::NetworkError e){
    /*
     * エラーコードについては
     * Qtリファレンスの
     * QNetworkReply Class -> enum QNetworkReply::NetworkError
     * に乗ってます
     */
    int code = e;
    std::cout << "network error code " << code  << std::endl;
    network_error_flag = true;
}

#endif // NET_H
