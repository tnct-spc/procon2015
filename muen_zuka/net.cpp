#include <QObject>
#include "net.hpp"
//コンストラクタでURL指定してね
net::net(QUrl server_url)
{
    _server_url = server_url;
    _master_url = server_url;
    _id = "0";
    _problem_num = 1;
}
net::net(QUrl server_url,QUrl master_url)
{
    _server_url = server_url;
    _master_url = master_url;
    _id = "0";
    _problem_num = 1;
}
net::net(QUrl server_url,QUrl master_url,std::string id,int problem_num){
    _server_url = server_url;
    _master_url = master_url;
    _id = id;
    _problem_num = problem_num;
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
    if(network_error_flag)return std::string("");
    return std::string(reply->readAll().constData());
}

std::string net::send(field_type answer){
    std::cout << "in send ()" << std::endl;
    QEventLoop eventloop;
    QUrlQuery postData;
    postData.addQueryItem("id",_id.c_str());
    postData.addQueryItem("quest_number",QString::number(_problem_num));
    postData.addQueryItem("answer",answer.get_answer().c_str());
    QNetworkRequest req(_master_url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
    QNetworkReply *reply = manager->post(req,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    eventloop.exec();
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
}