#include <QObject>
#include <iostream>
#include "net.hpp"
//コンストラクタでURL指定してね
net::net(QUrl server_url)
{
    _server_url = server_url;
    _master_url = server_url;
    _problem_num = 1;
}
net::net(QUrl server_url,QUrl master_url)
{
    _server_url = server_url;
    _master_url = master_url;
    _problem_num = 1;
}
net::net(QUrl server_url,QUrl master_url,int problem_num){
    _server_url = server_url;
    _master_url = master_url;
    _problem_num = problem_num;
}

//GETで取ってくる
std::string net::get()
{
    //manager = new QNetworkAccessManager(this);
    QEventLoop eventloop;

    connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
    QUrl requrl=_server_url.toString()+"/problem"+QString::number(_problem_num)+".txt";
    QNetworkReply *reply = manager->get(QNetworkRequest(requrl));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    eventloop.exec();
    if(network_error_flag)return std::string("");
    return std::string(reply->readAll().constData());
}

std::string net::send(field_type answer, QString playerid){
    QEventLoop eventloop;
    QUrlQuery postData;
    postData.addQueryItem("point",QString::number(answer.get_score()));
    postData.addQueryItem("quest_number",QString::number(_problem_num));
    postData.addQueryItem("answer",answer.get_answer().c_str());
    postData.addQueryItem("id",playerid);
    QNetworkRequest req(_master_url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
    QNetworkReply *reply = manager->post(req,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    eventloop.exec();
    qDebug(reply->readAll().constData());
    if(network_error_flag)return std::string("");
    return std::string(reply->readAll().constData());
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
    net_error_num = e;
}
bool net::is_error(){
    return network_error_flag;
}
int net::what_error(){
    return net_error_num;
}
