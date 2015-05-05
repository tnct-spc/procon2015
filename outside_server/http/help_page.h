#ifndef HELPPAGE_H
#define HELPPAGE_H

#include <httprequesthandler.h>

class HelpPage : public HttpRequestHandler
{
    Q_OBJECT
public:
    HelpPage(QObject* parent=0);
    void service(/*HttpRequest& request,*/ HttpResponse& response);
};

#endif // HELPPAGE_H
