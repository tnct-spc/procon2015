#include <QObject>
#include "net.hpp"
//コンストラクタでURL指定してね
net::net(QUrl server_url)
{
    manager->setCookieJar(&CJ);
    _server_url = server_url;
    _master_url = server_url;
    _problem_num = 1;
}
net::net(QUrl server_url,QUrl master_url)
{
    manager->setCookieJar(&CJ);
    _server_url = server_url;
    _master_url = master_url;
    _problem_num = 1;
}
net::net(QUrl server_url,QUrl master_url,int problem_num){
    manager->setCookieJar(&CJ);
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
    QNetworkReply *reply = manager->get(QNetworkRequest(_server_url));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    eventloop.exec();
    if(network_error_flag)return std::string("");
    return std::string(reply->readAll().constData());
}
/*
void net::login(){
    QUrlQuery postData;
    postData.addQueryItem("utf8","&#x2713;");
    postData.addQueryItem("utf8","&#x2713;");
    QNetworkRequest req(_master_url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");
}
*/
std::string net::send(field_type answer){
    /*
    QUrlQuery postData;
    postData.addQueryItem("utf8","&#x2713;");
    postData.addQueryItem("authenticity_token","CBvSfAe+i7p8EnW100civC2UdGEL2jYRlFMydvzQ2YFDQx/9mbMmJKzvL+jtOQ+WkQYZReG4btBkuj31Ua/m5A==");
    postData.addQueryItem("answer[text]",answer.get_answer().c_str());
    */
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data; name=\"answer[text]\"; filename=\"Ramen_jiro.txt\""));
    textPart.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("text/plain"));
    textPart.setBody(answer.get_answer().c_str());
    multiPart->append(textPart);
    QNetworkRequest req(_master_url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");
    connect(manager,&QNetworkAccessManager::finished,this,&net::replyfinished);
    QNetworkReply *reply = manager->post(req,multiPart);
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
    if(network_error_flag)return std::string("");
    return "sent";
}
void net::replyfinished(QNetworkReply* reply){
    std::cout << std::string(reply->readAll().constData()) << std::endl;
    QList<QByteArray> headerList = reply->rawHeaderList();
    foreach(QByteArray head, headerList) {
        qDebug() << head << ":" << reply->rawHeader(head);
    }
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
