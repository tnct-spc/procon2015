#ifndef NET_H
#define NET_H
#include "field_type.hpp"
#include <string>
#include <QObject>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QByteArray>
#include <QEventLoop>
#include <QUrlQuery>
#include <iostream>
#include <stdexcept>
#include <QSharedPointer>
#include <QHttpPart>
//コンストラクタでURL指定してね
class net : public QObject
{
    Q_OBJECT
public:
    explicit net(QObject *parent = 0);
    net(QUrl server_url); //スタンドアロンで動作する
    net(QUrl server_url,QUrl master_url);
    net(QUrl server_url, QUrl master_url, QString plyaer_id, int problem_num);
    ~net();
    std::string get();
    std::string get_from_official_server();
    std::string send(field_type answer);
    std::string send_to_official_server(field_type answer);
    std::string send_to_official_server(std::string answer_text);
    bool is_error();
    int what_error();
signals:

public slots:

private slots:
    void networkerror(QNetworkReply::NetworkError);
    void replyfinished(QNetworkReply* reply);
private:
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    //std::shared_ptr<QNetworkAccessManager> manager;
    QString random_naming();
    QUrl _server_url,_master_url;
    QString _player_id;
    int _problem_num;
    bool network_error_flag =false;
    enum QNetworkReply::NetworkError net_error_num = QNetworkReply::NoError;
};

#endif // NET_H
