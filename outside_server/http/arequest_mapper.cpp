#include "arequest_mapper.h"

RequestMapper::RequestMapper(QObject *parent) : QObject(parent) {
    QHttpServer *server = new QHttpServer;
    QObject::connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)), this, SLOT(service(QHttpRequest&,QHttpResponse&)));

    server->listen(8080);
}

void RequestMapper::service(QHttpRequest& request, QHttpResponse& response) {
    QString path=request.path();

    if (path=="/" || path=="/help") {
        P_help_page.service(response);
    }
    /*
    else if (path=="/answer") {
        P_answer_page.service(request, response);
    }
    else if (path.startsWith("/files")) {
        static_file_controller->service(request,response);
    }
    else {
        response.setStatus(404,"Not found");
        response.write("The URL is wrong, no such document.");
    }
    */
}

