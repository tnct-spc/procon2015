#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    QHttpServer *server = new QHttpServer;
    QObject::connect(server, SIGNAL(newRequest(QHttpRequest*,QHttpResponse*)), this, SLOT(Service(QHttpRequest*,QHttpResponse*)));

    server->listen(8081);
}

void Server::Service(QHttpRequest *request, QHttpResponse *response) {
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

void Server::ServiceRequestCompleted(QByteArray lowdata){
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
    g_answer_data.push_back(answer_data);

    //PointCheck
    bool upload_flag=1;
    for(unsigned int i=0;i<g_answer_data.size()-1;i++){
        if(post_problem_number == g_answer_data[i].problem_number && post_point >= g_answer_data[i].answer_point){
            upload_flag=0;
            break;
        }
    }

    //Send
    if(upload_flag){
        QEventLoop eventloop;
        QUrlQuery postData;
        postData.addQueryItem("id",g_token_name);
        postData.addQueryItem("quest_number",post_problem_number);
        postData.addQueryItem("answer",post_raw_answer_data);
        QNetworkRequest req(_master_url);
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
        response->write("Don't send to server.\n");
    }
    response->end();
}
