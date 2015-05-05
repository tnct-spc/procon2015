#include "help_page.h"

HelpPage::HelpPage(QObject* parent) : HttpRequestHandler(parent) {
    // empty
}

void HelpPage::service(/*HttpRequest &request,*/ HttpResponse &response) {
    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    response.write("<html><body>");
    response.write("<font size=15>");
    response.write("TNCT-SPC PROCON26 SERVER<br>");
    response.write("If you want to get the problem. Please access \"/files/problem(problemnumber).txt\".<br>");
    response.write("If you want to push the answer.Please access \"/answer\".<br>");
    response.write("</font>");
    response.write("</body></html>",true);
}
