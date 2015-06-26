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
}

Master::~Master()
{
    delete ui;
}

void Master::change_token_box(){
    token_name_ = ui->token_name_box->text();
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
    QString post_problem_number=url_query.queryItemValue("quest_number");
    QString post_raw_answer_data=url_query.queryItemValue("answer");

    //response head
    response->setHeader("Content-Type", "text/html; charset=UTF-8");
    response->writeHead(200);

    //Set
    answer_data_type answer_data;
    answer_data.answer_point=post_point;
    answer_data.answer_raw_data=post_raw_answer_data;
    answer_data.problem_number=post_problem_number;
    answer_data_.push_back(answer_data);

    //PointCheck
    bool upload_flag=1;
    for(unsigned int i=0;i<answer_data_.size()-1;i++){
        if(post_problem_number == answer_data_[i].problem_number && post_point >= answer_data_[i].answer_point){
            upload_flag=0;
            break;
        }
    }

    //Send
    if(upload_flag){
        QEventLoop eventloop;
        QUrlQuery postData;
        postData.addQueryItem("id",token_name_);
        postData.addQueryItem("quest_number",post_problem_number);
        postData.addQueryItem("answer",post_raw_answer_data);
        QNetworkRequest req(ui->send_url->text());
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        connect(manager,SIGNAL(finished(QNetworkReply*)),&eventloop,SLOT(quit()));
        QNetworkReply *reply = manager->post(req,postData.toString(QUrl::FullyEncoded).toUtf8());
        //connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkerror(QNetworkReply::NetworkError)));
        eventloop.exec();
        //if(network_error_flag)return std::string("");

        //Response
        response->write("response:");
        response->write(reply->readAll());
        response->write("\n");
    }else{
        response->write("no send\n");
    }
    response->end();
}
