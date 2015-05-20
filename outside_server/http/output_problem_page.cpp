#include "output_problem_page.h"

OutputProblemPage::OutputProblemPage(QObject *parent) : QObject(parent)
{

}

void OutputProblemPage::Service(QHttpRequest *request, QHttpResponse *response){
    //get addless
    QString addless =request->url().toString();

    //saerch filename
    int filelen=addless.length();
    int seek=filelen-1;
    while(addless.mid(seek,1)!="/") seek--;

    //open problem file
    QString filename_problem=ProblemFolderName+addless.mid(seek);
    QFile problem(filename_problem);
    if(problem.open(QIODevice::ReadOnly)){
        response->writeHead(200);
        response->write(problem.readAll());
        response->end();
        problem.close();
    }else{
        response->writeHead(404);
        response->write("404 Not Found.");
        response->end();
    }
}

