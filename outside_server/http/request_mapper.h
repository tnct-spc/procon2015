#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include <httprequesthandler.h>
#include <httpsessionstore.h>
#include <staticfilecontroller.h>
#include <httpsession.h>
#include <http/help_page.h>
#include <http/answer_form.h>

class RequestMapper : public HttpRequestHandler
{
    Q_OBJECT
public:
    RequestMapper(QObject* parent=0);
    void service(HttpRequest& request, HttpResponse& response);
    static HttpSessionStore* session_store;
    static StaticFileController* static_file_controller;

private:
    HelpPage P_help_page;
    AnswerForm P_answer_page;
};

#endif // REQUESTMAPPER_H

