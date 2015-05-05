#include "request_mapper.h"

HttpSessionStore* RequestMapper::session_store=0;
StaticFileController* RequestMapper::static_file_controller=0;

RequestMapper::RequestMapper(QObject* parent) : HttpRequestHandler(parent) {
    // empty
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
    QByteArray path=request.getPath();

    if (path=="/" || path=="/help") {
        P_help_page.service(response);
    }
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
}

