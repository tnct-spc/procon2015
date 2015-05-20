#include "request_mapper.h"

RequestMapper::RequestMapper(QObject *parent) : QObject(parent){
    QHttpServer *server = new QHttpServer;
    QObject::connect(server, SIGNAL(newRequest(QHttpRequest*,QHttpResponse*)), this, SLOT(service(QHttpRequest*,QHttpResponse*)));

    server->listen(8080);
}

void RequestMapper::service(QHttpRequest* request, QHttpResponse* response) {
    QString path=request->path();

    if (path=="/" || path=="/help") {
        P_help_page.Service(response);
    }
    else if (path=="/answer") {
        P_answer_page.Service(request, response);
    }
    else if (path.startsWith("/problem")) {
        P_output_problem_page.Service(request, response);
    }
    else {
        response->writeHead(404);
        response->write("404 Not Found.");
        response->end();
    }
}
