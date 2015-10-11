#include "boost/algorithm/string.hpp"
#include "master.h"
#include "ui_master.h"

Master::Master(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Master)
{
    ui->setupUi(this);

    QHttpServer *server = new QHttpServer;
    QObject::connect(server, SIGNAL(newRequest(QHttpRequest*,QHttpResponse*)), this, SLOT(Service(QHttpRequest*,QHttpResponse*)));

    server->listen(8081);

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

void Master::Service(QHttpRequest *request, QHttpResponse *response) {
    new_response_=response;

    if(request->method()==QHttpRequest::HTTP_POST){
        if(request->body().isEmpty()){
            //Wait get body
            connect(request,SIGNAL(data(QByteArray)),this,SLOT(ServiceRequestCompleted(QByteArray)));
        }else{
            ServiceRequestCompleted(request->body());
        }
    }else{
        //Form

        response->setHeader("Content-Type", "text/html; charset=UTF-8");
        response->writeHead(200);
        response->write("<html><head><title>MASTER FORM</title></head><body>");
        response->write("<form method='POST' action='/answer'>");
        response->write("Master From:<br>");
        response->write("Point:  <input type='text' name='point'><br>");
        response->write("ProblemNumber: <input type='text' name='quest_number'><br>");
        response->write("Answer: <textarea name='answer' cols='100' rows='40'></textarea><br>");
        response->write("<input type='submit'>");
        response->write("</form>");
        response->write("</body></html>");

        response->end();

        response->end();
    }
}

void Master::ServiceRequestCompleted(QByteArray lowdata){
    QHttpResponse *response=new_response_;

    //Get request data
    QUrlQuery url_query(lowdata);
    int post_point=url_query.queryItemValue("point").toInt();
    int post_processes_size=url_query.queryItemValue("processes_size").toInt();
    QString post_problem_number=url_query.queryItemValue("quest_number");
    QString post_raw_answer_data=url_query.queryItemValue("answer");
    QString slave_name = url_query.queryItemValue("id");

    //encode
    std::string post_answer_data_encoded = boost::algorithm::replace_all_copy(post_raw_answer_data.toStdString(),"%0D%0A","\r\n");

    //response head
    response->setHeader("Content-Type", "text/html; charset=UTF-8");
    response->writeHead(200);

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
            //if(network_error_flag)return std::string("");

            //Response
            response->write(reply->readAll());
        }
        response->write("Master : send ok.\n");
    }else{
        response->write("Master : no send.\n");
    }
    response->end();
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
