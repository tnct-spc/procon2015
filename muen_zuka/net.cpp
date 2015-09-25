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
net::net(QUrl server_url,QUrl master_url, QString player_id, int problem_num){
    _server_url = server_url;
    _master_url = master_url;
    _player_id = player_id;
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
std::string net::get_from_official_server(){
    QEventLoop eventloop;
    connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
    QUrl requrl=_server_url.toString()+"/quest"+QString::number(_problem_num)+".txt?token=0123456789abcdef";
    //66b77ce56fd29d27
    QNetworkReply *reply = manager->get(QNetworkRequest(requrl));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    eventloop.exec();
    if(network_error_flag)return std::string("");
    return std::string(reply->readAll().constData());
}

std::string net::send(field_type answer){
    QEventLoop eventloop;
    QUrlQuery postData;
    postData.addQueryItem("point",QString::number(answer.get_score()));
    postData.addQueryItem("quest_number",QString::number(_problem_num));
    postData.addQueryItem("answer",answer.get_answer().c_str());
    postData.addQueryItem("id",_player_id);
    QNetworkRequest req(_master_url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
    QNetworkReply *reply = manager->post(req,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    eventloop.exec();
    qDebug() << QString(reply->readAll().constData());
    if(network_error_flag)return std::string("");
    return std::string(reply->readAll().constData());
}
std::string net::send_to_official_server(field_type answer){
    QEventLoop eventloop;
    QHttpMultiPart multi_part(QHttpMultiPart::FormDataType);
    QHttpPart token_part;
    token_part.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data; name=\"token\""));
    token_part.setBody("0123456789abcdef");
    //66b77ce56fd29d27
    QHttpPart answer_part;
    answer_part.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data; name=\"answer\"; filename=\"Phantasmagoria_of_Flower_View.txt\""));
    answer_part.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("text/plain"));
    answer_part.setBody(answer.get_answer().c_str());
    multi_part.append(token_part);
    multi_part.append(answer_part);
    QUrl req_url = _master_url.toString()+"/answer";
    QNetworkRequest req(req_url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(QString("multipart/form-data; boundary=") + QString(multi_part.boundary())));
    connect(manager,&QNetworkAccessManager::finished,&eventloop,&QEventLoop::quit);
    QNetworkReply *reply = manager->post(req,&multi_part);
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    eventloop.exec();
    qDebug() << QString(reply->readAll().constData());
    if(network_error_flag)return std::string("");
    return "sent";
}

//デストラクタ書いてないからメモリリークするよ
//🍣　ご　め　ん　ね　🍣//
//追記 多分もうしない
net::~net()
{
    delete manager;

}
void net::replyfinished(QNetworkReply* reply){
    std::cout << std::string(reply->readAll().constData()) << std::endl;
    QList<QByteArray> headerList = reply->rawHeaderList();
    foreach(QByteArray head, headerList) {
        qDebug() << head << ":" << reply->rawHeader(head);
    }
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
