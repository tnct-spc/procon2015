#ifndef ANSWERFORM_H
#define ANSWERFORM_H

#include <httprequesthandler.h>
#include <QCoreApplication>
#include <global.h>

class AnswerForm : public HttpRequestHandler
{
    Q_OBJECT
public:
    AnswerForm(QObject* parent=0);
    void service(HttpRequest& request, HttpResponse& response);
    //return point of answer
    QString SimulateAnswerPoint(QString answer_file_name);
private:
    //put a stone on stage
    bool PutStone();


    //pass
    QString AnswerFolderName=QCoreApplication::applicationDirPath()+"/etc/answer/";

    //stage data
    int stage_state_[48][48];//0=empty;1=block;2=answer_block
    bool stone_state_[256][8][8];
    int stone_num_;
    //answer data
    int answer_flow_[256][4];
    int answer_num_;
    int stone_flow_count_;


};

#endif // ANSWERFORM_H
