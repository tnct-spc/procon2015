#ifndef OUTPUTPROBLEMPAGE_H
#define OUTPUTPROBLEMPAGE_H

#include <QObject>
#include <QCoreApplication>
#include <QFile>
#include <src/qhttpserver.h>
#include <src/qhttprequest.h>
#include <src/qhttpresponse.h>

class OutputProblemPage : public QObject
{
    Q_OBJECT
public:
    explicit OutputProblemPage(QObject *parent = 0);
    void Service(QHttpRequest* request, QHttpResponse* response);

private:
    //pass
    QString ProblemFolderName=QCoreApplication::applicationDirPath()+"/docroot/files/";
};

#endif // OUTPUTPROBLEMPAGE_H
