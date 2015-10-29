#include "boost/algorithm/string.hpp"
#include "master.h"
#include "ui_master.h"

#define BOOSTCONNECT_LIB_BUILD
#include <boostconnect/server.hpp>
#include <boostconnect/session_type/http_session.hpp>
#include <boostconnect/session_type/impl/http_session.ipp>
#include <boostconnect/utility/impl/http.ipp>
#include <boostconnect/application_layer/impl/tcp_socket.ipp>
#include <boostconnect/connection_type/impl/async_connection.ipp>

Master::Master(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Master),
    io_service_(boost::make_shared<boost::asio::io_service>()),
    server_(boost::make_shared<http_server>(io_service_, 8081))
{
    ui->setupUi(this);

    // Serverの開始と，実行スレッドのぶん投げ
    server_->start(std::bind(&Master::Service, this, std::placeholders::_1, std::placeholders::_2));
    thread_.reset(
        new std::thread([&]{ io_service_->run(); })
        );

    //setting load
    settings = new QSettings("setting.ini",QSettings::IniFormat);
    settings->beginGroup("SETTING");
    if(!QFile::exists("setting.ini")){
        //init
        settings->setValue("SELECT_SEND",1);
        settings->setValue("CHANGE_URL_1","http://127.0.0.1:8080/answer");
        settings->setValue("CHANGE_URL_2","http://127.0.0.1:8080/answer");
        settings->setValue("CHANGE_URL_3","http://127.0.0.1:8080/answer");
    }
    switch(settings->value("SELECT_SEND").toInt()){
    case 1:
        ui->selecturl_button_1->setChecked(true);
        break;
    case 2:
        ui->selecturl_button_2->setChecked(true);
        break;
    case 3:
        ui->selecturl_button_3->setChecked(true);
        break;
    }
    ui->send_url_1->setText(settings->value("CHANGE_URL_1").toString());
    ui->send_url_2->setText(settings->value("CHANGE_URL_2").toString());
    ui->send_url_3->setText(settings->value("CHANGE_URL_3").toString());
}

Master::~Master()
{
    delete ui;
}

void Master::change_token_box(){
    token_name_ = ui->token_name_box->text();
}

void Master::reset_point(){
    answer_data_.clear();
}

void Master::Service(boost::shared_ptr<request_type> const req, session_ptr session)
{
    if(req->method == "POST")
    {
        // ソルバーから解答を受信
        auto response = boost::make_shared<bstcon::response>();
        response->http_version = "1.1";
        response->status_code = 200;
        response->reason_phrase = "OK";
        response->body = "Master : OK\r\n";
        response->header["Content-Length"] = std::to_string(response->body.length());
        response->header["Content-Type"] = "text/html; charset=UTF-8";
        session->set_all(response);

        // Requestをもらったことを，比較器に通知g
        ServiceRequestCompleted(req->body.c_str());
    }
    else
    {
        // 移植元がこうだったので従ったけれど，
        // よくわからないので，解る人，コメントお願いいたします
        auto response = boost::make_shared<bstcon::response>();
        response->http_version = "1.1";
        response->status_code = 200;
        response->reason_phrase = "OK";
        response->body = "<html><head><title>MASTER FORM</title></head><body>\r\n"
                         "<form method='POST' action='/answer'>\r\n"
                         "Master From:<br>\r\n"
                         "Point:  <input type='text' name='point'><br>\r\n"
                         "ProblemNumber: <input type='text' name='quest_number'><br>\r\n"
                         "Answer: <textarea name='answer' cols='100' rows='40'></textarea><br>\r\n"
                         "<input type='submit'>\r\n"
                         "</form>\r\n"
                         "</body></html>\r\n";
        response->header["Content-Length"] = std::to_string(response->body.length());
        response->header["Content-Type"] = "text/html; charset=UTF-8";
        session->set_all(response);
    }
}

void Master::ServiceRequestCompleted(QByteArray lowdata){
    //Get request data
    QUrlQuery url_query(lowdata);
    int post_point=url_query.queryItemValue("point").toInt();
    int post_processes_size=url_query.queryItemValue("processes_size").toInt();
    QString post_problem_number=url_query.queryItemValue("quest_number");
    QString post_raw_answer_data=url_query.queryItemValue("answer");
    QString slave_name = url_query.queryItemValue("id");

    //Set
    answer_data_type answer_data;
    answer_data.answer_point=post_point;
    answer_data.answer_processes_size=post_processes_size;
    answer_data.answer_raw_data=post_raw_answer_data;
    answer_data.problem_number=post_problem_number;
    answer_data_.push_back(answer_data);

    //PointCheck
    bool upload_flag=1;
    for(unsigned int i=0;i<answer_data_.size()-1;i++){
        if(post_problem_number == answer_data_[i].problem_number && (post_point > answer_data_[i].answer_point || (post_point == answer_data_[i].answer_point && post_processes_size >= answer_data_[i].answer_processes_size))){
            upload_flag=0;
            break;
        }
    }
    //Send
    if(upload_flag){
        qDebug("%s : send. score=%d",slave_name.toStdString().c_str(),answer_data.answer_point);
    }else{
        qDebug("%s : nosend. score=%d",slave_name.toStdString().c_str(),answer_data.answer_point);
    }
    if(upload_flag){
        if(ui->checkBox_sendOfficialServer->isChecked()){
            //encode
            std::string post_answer_data_encoded = boost::algorithm::replace_all_copy(post_raw_answer_data.toStdString(),"%0D%0A","\r\n");

            net network(QString(""),get_sendurl());
            network.send_to_official_server(post_answer_data_encoded);
        }else{
            QEventLoop eventloop;
            QUrlQuery postData;
            postData.addQueryItem("id",token_name_);
            postData.addQueryItem("quest_number",post_problem_number);
            postData.addQueryItem("answer",post_raw_answer_data);
            QNetworkRequest req(get_sendurl());
            req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
            QNetworkReply *reply = manager->post(req,postData.toString(QUrl::FullyEncoded).toUtf8());
            //connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
            eventloop.exec();
        }
    }
}

QString Master::get_sendurl(){
    if(ui->selecturl_button_1->isChecked()) return ui->send_url_1->text();
    if(ui->selecturl_button_2->isChecked()) return ui->send_url_2->text();
    if(ui->selecturl_button_3->isChecked()) return ui->send_url_3->text();
    return "";
}

void Master::select_send_1(){
    settings->setValue("SELECT_SEND",1);
}
void Master::select_send_2(){
    settings->setValue("SELECT_SEND",2);
}
void Master::select_send_3(){
    settings->setValue("SELECT_SEND",3);
}
void Master::change_url_1(){
    settings->setValue("CHANGE_URL_1",ui->send_url_1->text());
}
void Master::change_url_2(){
    settings->setValue("CHANGE_URL_2",ui->send_url_2->text());
}
void Master::change_url_3(){
    settings->setValue("CHANGE_URL_3",ui->send_url_3->text());
}
